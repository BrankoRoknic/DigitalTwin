#include "CesiumClient.h"

static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char reverse_table[128] = {
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

std::string base64_encode(const std::string& bindata)
{
	using std::string;
	using std::numeric_limits;

	if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
		throw ::std::length_error("Converting too large a string to base64.");
	}

	const ::std::size_t binlen = bindata.size();
	// Use = signs so the end is properly padded.
	string retval((((binlen + 2) / 3) * 4), '=');
	::std::size_t outpos = 0;
	int bits_collected = 0;
	unsigned int accumulator = 0;
	const string::const_iterator binend = bindata.end();

	for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
		accumulator = (accumulator << 8) | (*i & 0xffu);
		bits_collected += 8;
		while (bits_collected >= 6) {
			bits_collected -= 6;
			retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
		}
	}
	if (bits_collected > 0) { // Any trailing bits that are missing.
		assert(bits_collected < 6);
		accumulator <<= 6 - bits_collected;
		retval[outpos++] = b64_table[accumulator & 0x3fu];
	}
	assert(outpos >= (retval.size() - 2));
	assert(outpos <= retval.size());
	return retval;
}

std::string hmac_sha256(const std::string& key, const std::string& data) {
	UE_LOG(LogTemp, Error, TEXT("HMAC: %s, %s"), UTF8_TO_TCHAR(key.c_str()), UTF8_TO_TCHAR(data.c_str()));

	unsigned char* digest = HMAC(EVP_sha256(), key.c_str(), key.length(), (unsigned char*)data.c_str(), data.length(), NULL, NULL);
	return std::string((char*)digest, 32); // SHA256 produces a 32-byte hash
}

// Function to convert byte array to hex string
std::string bytesToHexString(const unsigned char* bytes, size_t length) {
	std::ostringstream oss;
	for (size_t i = 0; i < length; ++i) {
		oss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];
	}
	return oss.str();
}

// Function to compute SHA256 hash
std::string sha256(const unsigned char* data, size_t length) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, data, length);
	SHA256_Final(hash, &sha256);
	return bytesToHexString(hash, SHA256_DIGEST_LENGTH);
}

UCesiumClient::UCesiumClient()
{
	// This field variable contains the access key from Cesium
	fCesiumToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiIwM2MzYTRlNC04MzMzLTRhMDktODVjZS00Mjc0NWRjNGYyNjAiLCJpZCI6MjEzODI0LCJpYXQiOjE3MjE5ODk4MjV9.aDuw8NxL3XgyrWkZ7oqmhX6ImPXJgUG8ZCnxu--UPDs";
}

void UCesiumClient::UploadFile(FString aFile, FString aName, FString aConversionType, FString aProvidedDataType)
{

	// Get the HTTP module
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL("https://api.cesium.com/v1/assets");
	Request->SetVerb("POST");

	// Set headers
	FString token = "Bearer " + this->fCesiumToken;
	Request->SetHeader("Authorization", token);
	Request->SetHeader("Content-Type", "application/json");

	// Set request payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("name", aName);
	JsonObject->SetStringField("description", "");
	JsonObject->SetStringField("type", aConversionType);   // This is specifying what you want cesium to do with the uploaded data - IE create 3D tiles

	TSharedPtr<FJsonObject> OptionsObject = MakeShareable(new FJsonObject);
	OptionsObject->SetStringField("sourceType", aProvidedDataType);  // This is specifying what data you are physically providing to Cesium (.las/.tif etc)
	JsonObject->SetObjectField("options", OptionsObject);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);

	// Bind the response callback
	this->fFileName = aFile;
	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::ProvideS3BucketData);

	// Execute the request
	Request->ProcessRequest();

}

void UCesiumClient::ProvideS3BucketData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	if (!wasSuccessful || !response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Request Failed"));
		return;
	}

	FString data = response->GetContentAsString();
	UE_LOG(LogTemp, Display, TEXT("HTTP response from Cesium: %s"), *data);

	// Parse the JSON response
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(data);
	TSharedPtr<FJsonObject> jsonObject;

	if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON"));
		return;
	}

	TSharedPtr<FJsonObject> uploadLocation = jsonObject->GetObjectField("uploadLocation");

	FString endpoint = uploadLocation->GetStringField("endpoint");
	FString bucket = uploadLocation->GetStringField("bucket");
	FString prefix = uploadLocation->GetStringField("prefix");
	FString accessKey = uploadLocation->GetStringField("accessKey");
	FString secretAccessKey = uploadLocation->GetStringField("secretAccessKey");
	FString sessionToken = uploadLocation->GetStringField("sessionToken");

	UE_LOG(LogTemp, Log, TEXT("Endpoint: %s"), *endpoint);
	UE_LOG(LogTemp, Log, TEXT("Bucket: %s"), *bucket);
	UE_LOG(LogTemp, Log, TEXT("Prefix: %s"), *prefix);
	UE_LOG(LogTemp, Log, TEXT("AccessKey: %s"), *accessKey);
	UE_LOG(LogTemp, Log, TEXT("SecretAccessKey: %s"), *secretAccessKey);
	UE_LOG(LogTemp, Log, TEXT("SessionToken: %s"), *sessionToken);

	// Ensure the file exists
	FString filePath = this->fFileName;
	if (!FPaths::FileExists(filePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File not found: %s"), *filePath);
		return;
	}

	// Load file content
	TArray<uint8> fileContent;
	if (!FFileHelper::LoadFileToArray(fileContent, *filePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *filePath);
		return;
	}

	// Get file name and S3 path
	FString FileName = FPaths::GetCleanFilename(filePath);
	FString S3Path = FString::Printf(TEXT("%s%s"), *prefix, *FileName);

	// Generate the S3 URL
	FString S3Url = FString::Printf(TEXT("https://%s.s3.amazonaws.com/%s"), *bucket, *S3Path);

	// Prepare the PUT request
	FString contentType = "application/octet-stream";

	// Get the HTTP module
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = Http->CreateRequest();

	//endpoint = "https://enjib4zsgqqrr.x.pipedream.net/"; // TODO: remove

	FString url = endpoint + S3Path;

	UE_LOG(LogTemp, Error, TEXT("%s"), *url);

	HttpRequest->SetURL(url);
	HttpRequest->SetVerb("PUT");
	HttpRequest->SetHeader("Host", bucket + ".s3.amazonaws.com");
	HttpRequest->SetHeader("Content-Type", contentType);
	HttpRequest->SetContent(fileContent);

	// AWS Signature Version 4 signing process
	FString httpDate = FDateTime::UtcNow().ToHttpDate();
	FString isoDatetime = FDateTime::UtcNow().ToFormattedString(TEXT("%Y%m%dT%H%M%S%Z"));
	FString isoDate = FDateTime::UtcNow().ToFormattedString(TEXT("%Y%m%d"));

	FString STSPath = FDateTime::UtcNow().ToFormattedString(TEXT("%Y%m%d/us-east-1/s3/aws4_request"));

	//FString canonicalRequest = FString::Printf(TEXT("PUT\n%s\n%s\nx-amz-security-token:%s\n/%s/%s"), *contentType, *date, *sessionToken, *bucket, *S3Path);

	// Create the canonical request
	FString contentHash = UTF8_TO_TCHAR(sha256(fileContent.GetData(), fileContent.Num()).c_str());

	FString canonicalRequest = FString::Printf(TEXT("PUT\n/%s\n\ndate:%s\nhost:%s.s3.amazonaws.com\nx-amz-content-sha256:%s\nx-amz-security-token:%s\n\ndate;host;x-amz-content-sha256;x-amz-security-token\n%s"), *S3Path, *httpDate, *bucket, *contentHash, *sessionToken, *contentHash);
	UE_LOG(LogTemp, Error, TEXT(".\n.\nCanonical Request: %s"), *canonicalRequest);

	// Create the String to Sign
	FString canonicalRequestHash = UTF8_TO_TCHAR(sha256((unsigned char*)TCHAR_TO_UTF8(*canonicalRequest), canonicalRequest.Len()).c_str());

	FString stringToSign = FString::Printf(TEXT("AWS4-HMAC-SHA256\n%s\n%s\n%s"), *httpDate, *STSPath, *canonicalRequestHash);

	UE_LOG(LogTemp, Error, TEXT("STS: %s"), *stringToSign);

	// Create the signing key

	std::string signingKey = hmac_sha256(hmac_sha256(hmac_sha256(hmac_sha256(TCHAR_TO_UTF8(*FString::Printf(TEXT("AWS4%s"), *secretAccessKey)), TCHAR_TO_UTF8(*isoDate)), "us-east-1"), "s3"), "aws4_request");
	std::string hmacResult = hmac_sha256(signingKey, TCHAR_TO_UTF8(*stringToSign));

	// Base64 encode the result
	FString signature = UTF8_TO_TCHAR(bytesToHexString(reinterpret_cast<const unsigned char*>(hmacResult.c_str()), hmacResult.size()).c_str());
	FString signedHeaders = "date;host;x-amz-content-sha256;x-amz-security-token";

	// Add the Authorization header
	FString AuthorizationHeader = FString::Printf(TEXT("AWS4-HMAC-SHA256 Credential=%s/%s,SignedHeaders=%s,Signature=%s"), *accessKey, *STSPath, *signedHeaders, *signature);

	UE_LOG(LogTemp, Error, TEXT("%s"), *AuthorizationHeader);
	HttpRequest->SetHeader("Date", *httpDate);
	HttpRequest->SetHeader("Authorization", AuthorizationHeader);
	HttpRequest->SetHeader("x-amz-content-sha256", contentHash);
	UE_LOG(LogTemp, Error, TEXT("ghi"));
	if (!sessionToken.IsEmpty())
	{
		HttpRequest->SetHeader("x-amz-security-token", sessionToken);
	}

	// Bind to the response event
	HttpRequest->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			UE_LOG(LogTemp, Error, TEXT("jkl"));
			if (bWasSuccessful && Response->GetResponseCode() == 200)
			{
				UE_LOG(LogTemp, Log, TEXT("File successfully uploaded to S3."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to upload file to S3. Response code: %d Content %s"), Response->GetResponseCode(), *Response->GetContentAsString());
			}
		});

	// Execute the request
	//HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::NotifyCesiumUploadComplete);
	HttpRequest->ProcessRequest();
}

void UCesiumClient::NotifyCesiumUploadComplete(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	//fix 
}