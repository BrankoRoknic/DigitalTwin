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
    // LOG_CODE_5001: UCesiumClient constructor called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_5001: UCesiumClient constructor called."));

	// Load the Cesium Ion token from the config file
	fCesiumToken = LoadCesiumTokenFromConfig();

	if (fCesiumToken.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Cesium token is missing! Please set it in the configuration file."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Cesium token loaded successfully."));
	}
    // LOG_CODE_5002: Cesium token set
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_5002: Cesium token set successfully."));

    // Add default ignored assets to the ignored assets array
    fIgnoredAssets.Add(FString("Cesium World Terrain"));
    fIgnoredAssets.Add(FString("Bing Maps Aerial"));
    fIgnoredAssets.Add(FString("Bing Maps Aerial with Labels"));
    fIgnoredAssets.Add(FString("Bing Maps Road"));
    fIgnoredAssets.Add(FString("Cesium OSM Buildings"));
    fIgnoredAssets.Add(FString("Google Photorealistic 3D Tiles"));

    // LOG_CODE_5003: Ignored assets initialized
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_5003: Ignored assets array initialized with default assets."));
}
// Function to load the Cesium Ion token from the config file
FString UCesiumClient::LoadCesiumTokenFromConfig()
{
	FString Token;
	if (GConfig)
	{
		// Read the token from the DefaultGame.ini file
		GConfig->GetString(*ConfigSection, *ConfigKey, Token, GGameIni);
	}
	return Token;
}

// Function to set and save the Cesium Ion token in the config file
void UCesiumClient::SetCesiumToken(FString NewToken)
{
	fCesiumToken = NewToken;

	if (GConfig)
	{
		GConfig->SetString(*ConfigSection, *ConfigKey, *NewToken, GGameIni);
		GConfig->Flush(false, GGameIni);  // Save changes to the config file
	}
}

void UCesiumClient::UploadFile(FString aFile, FString aName, FString aConversionType, FString aProvidedDataType)
{
	// LOG_CODE_6001: UploadFile method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6001: UploadFile method called with parameters - aFile: %s, aName: %s, aConversionType: %s, aProvidedDataType: %s"), *aFile, *aName, *aConversionType, *aProvidedDataType);

	// Get the HTTP module
	FHttpModule* Http = &FHttpModule::Get();
	 if (!Http)
    {
        // LOG_CODE_6002: Failed to retrieve HTTP module
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_6002: Failed to retrieve HTTP module."));
        return;
    }
	// LOG_CODE_6003: HTTP module successfully retrieved
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6003: HTTP module successfully retrieved."));

	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL("https://api.cesium.com/v1/assets");
	Request->SetVerb("POST");

	 // LOG_CODE_6004: HTTP POST request created
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6004: HTTP POST request created for URL: https://api.cesium.com/v1/assets"));

	// Set headers
	FString token = "Bearer " + this->fCesiumToken;
	Request->SetHeader("Authorization", token);
	Request->SetHeader("Content-Type", "application/json");

	// LOG_CODE_6005: Headers set for HTTP request
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6005: Authorization: %s, Content-Type: application/json"), *token);

	// Set request payload
	char* lPath = TCHAR_TO_ANSI(*aFile);
	std::filesystem::path p(lPath);
	// Use p.filename() if you want the extension included in future

	FString fileName = p.stem().string().c_str();
	 // LOG_CODE_6007: Processed file name from path
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6007: Processed file name from path: %s"), *fileName);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("name", fileName);
	JsonObject->SetStringField("description", "");
	JsonObject->SetStringField("type", aConversionType);   // This is specifying what you want cesium to do with the uploaded data - IE create 3D tiles

    // LOG_CODE_6008: JSON object created
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6008: JSON object created with name: %s, type: %s"), *fileName, *aConversionType);

	TSharedPtr<FJsonObject> OptionsObject = MakeShareable(new FJsonObject);
	OptionsObject->SetStringField("sourceType", aProvidedDataType);  // This is specifying what data you are physically providing to Cesium (.las/.tif etc)
	JsonObject->SetObjectField("options", OptionsObject);
	// LOG_CODE_6009: Options JSON object added
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6009: Options JSON object added with sourceType: %s"), *aProvidedDataType);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	// LOG_CODE_6011: JSON payload serialized successfully
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6011: JSON payload serialized successfully: %s"), *RequestBody);


	Request->SetContentAsString(RequestBody);

	// Bind the response callback
	this->fFileName = aFile;
	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::ProvideS3BucketData);
	// LOG_CODE_6012: Response callback bound
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_6012: Response callback bound to ProvideS3BucketData."));

	// Execute the request
	Request->ProcessRequest();

}

void UCesiumClient::ProvideS3BucketData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	// LOG_CODE_7001: ProvideS3BucketData method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7001: ProvideS3BucketData method called."));

	if (!wasSuccessful || !response.IsValid())
	{
		 // LOG_CODE_7002: Upload with Cesium failed
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_7002: Failed to start upload with Cesium."));
        return;
	}
	else if (response->GetResponseCode() != 200)
	{
		 // LOG_CODE_7003: Non-200 response code received from Cesium
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_7003: Failed to start upload with Cesium. Response code: %d Content: %s"), response->GetResponseCode(), *response->GetContentAsString());
        return;
	}

	FString data = response->GetContentAsString();
	// LOG_CODE_7004: Valid HTTP response received from Cesium
    UE_LOG(LogTemp, Display, TEXT("LOG_CODE_7004: HTTP response from Cesium: %s"), *data);


	// Parse the JSON response
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(data);
	TSharedPtr<FJsonObject> jsonObject;

	if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
	{
		// LOG_CODE_7005: JSON parsing failed
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_7005: Failed to parse JSON."));
        return;
	}
	// LOG_CODE_7006: JSON parsed successfully
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7006: JSON parsed successfully."));

	// Set some field variables to be used in a later step from this response - Notify cesium on upload complete.
	TSharedPtr<FJsonObject> onComplete = jsonObject->GetObjectField("onComplete");
	fNotifyCompleteVerb = onComplete->GetStringField("method");
	fNotifyCompleteURL = onComplete->GetStringField("url");

	 // LOG_CODE_7007: OnComplete data extracted
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7007: OnComplete - Method: %s, URL: %s"), *fNotifyCompleteVerb, *fNotifyCompleteURL);

	// Set some local variables to be used in constructing the S3 API Request
	TSharedPtr<FJsonObject> uploadLocation = jsonObject->GetObjectField("uploadLocation");
	FString endpoint = uploadLocation->GetStringField("endpoint");
	FString bucket = uploadLocation->GetStringField("bucket");
	FString prefix = uploadLocation->GetStringField("prefix");
	FString accessKey = uploadLocation->GetStringField("accessKey");
	FString secretAccessKey = uploadLocation->GetStringField("secretAccessKey");
	FString sessionToken = uploadLocation->GetStringField("sessionToken");
	   // LOG_CODE_7008: Upload location data extracted
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7008: UploadLocation - Endpoint: %s, Bucket: %s, Prefix: %s, AccessKey: %s, SecretAccessKey: %s, SessionToken: %s"), *endpoint, *bucket, *prefix, *accessKey, *secretAccessKey, *sessionToken);


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
		 // LOG_CODE_7009: File not found
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_7009: File not found: %s"), *filePath);
        return;
	}
	 
	// LOG_CODE_7010: File found, proceeding with upload
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7010: File found at path: %s"), *filePath);

	// Load file content
	TArray<uint8> fileContent;
	if (!FFileHelper::LoadFileToArray(fileContent, *filePath))
	{
		// LOG_CODE_7011: Failed to load file
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_7011: Failed to load file: %s"), *filePath);
        return;
	}
	fFileSize = fileContent.Num();

	 // LOG_CODE_7012: File loaded successfully
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7012: File loaded successfully. File size: %d bytes"), fFileSize);

	// Get file name and S3 path
	FString FileName = FPaths::GetCleanFilename(filePath);
	FString S3Path = FString::Printf(TEXT("%s%s"), *prefix, *FileName);

	 // LOG_CODE_7013: S3 path generated
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7013: S3Path generated: %s"), *S3Path);

	// Generate the S3 URL
	FString S3Url = FString::Printf(TEXT("https://%s.s3.amazonaws.com/%s"), *bucket, *S3Path);

	// LOG_CODE_7014: S3 URL generated
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7014: S3 URL generated: %s"), *S3Url);

	// Prepare the PUT request
	FString contentType = "application/octet-stream";

	// Get the HTTP module
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	FString url = endpoint + S3Path;

	Request->SetURL(url);
	Request->SetVerb("PUT");
	Request->SetHeader("Host", bucket + ".s3.amazonaws.com");
	Request->SetHeader("Content-Type", contentType);
	Request->SetContent(fileContent);

	// LOG_CODE_7015: PUT request prepared for S3
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7015: PUT request prepared for S3 upload. URL: %s, Content-Type: %s"), *url, *contentType);

	// AWS Signature Version 4 signing process
	FString httpDate = FDateTime::UtcNow().ToHttpDate();
	FString isoDatetime = FDateTime::UtcNow().ToFormattedString(TEXT("%Y%m%dT%H%M%S%Z"));
	FString isoDate = FDateTime::UtcNow().ToFormattedString(TEXT("%Y%m%d"));
	FString STSPath = FString::Printf(TEXT("%s/us-east-1/s3/aws4_request"), *isoDate);

	// LOG_CODE_7016: AWS signing data generated
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7016: AWS signing data generated - HttpDate: %s, IsoDatetime: %s, IsoDate: %s"), *httpDate, *isoDatetime, *isoDate);

	// Create the canonical request
	FString contentHash = UTF8_TO_TCHAR(sha256(fileContent.GetData(), fileContent.Num()).c_str());

	FString canonicalRequest = FString::Printf(TEXT("PUT\n/%s\n\ndate:%s\nhost:%s.s3.amazonaws.com\nx-amz-content-sha256:%s\nx-amz-security-token:%s\n\ndate;host;x-amz-content-sha256;x-amz-security-token\n%s"), *S3Path, *httpDate, *bucket, *contentHash, *sessionToken, *contentHash);

	// LOG_CODE_7017: Canonical request created
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7017: Canonical request created."));

	// Create the String to Sign
	FString canonicalRequestHash = UTF8_TO_TCHAR(sha256((unsigned char*)TCHAR_TO_UTF8(*canonicalRequest), canonicalRequest.Len()).c_str());

	FString stringToSign = FString::Printf(TEXT("AWS4-HMAC-SHA256\n%s\n%s\n%s"), *httpDate, *STSPath, *canonicalRequestHash);

	// Create the signing key
	// LOG_CODE_7018: String to Sign created
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7018: String to Sign created."));

	std::string signingKey = hmac_sha256(hmac_sha256(hmac_sha256(hmac_sha256(TCHAR_TO_UTF8(*FString::Printf(TEXT("AWS4%s"), *secretAccessKey)), TCHAR_TO_UTF8(*isoDate)), "us-east-1"), "s3"), "aws4_request");
	std::string hmacResult = hmac_sha256(signingKey, TCHAR_TO_UTF8(*stringToSign));

	FString signature = UTF8_TO_TCHAR(bytesToHexString(reinterpret_cast<const unsigned char*>(hmacResult.c_str()), hmacResult.size()).c_str());
	FString signedHeaders = "date;host;x-amz-content-sha256;x-amz-security-token";

	// Add the Authorization header
	FString AuthorizationHeader = FString::Printf(TEXT("AWS4-HMAC-SHA256 Credential=%s/%s,SignedHeaders=%s,Signature=%s"), *accessKey, *STSPath, *signedHeaders, *signature);

	Request->SetHeader("Date", *httpDate);
	Request->SetHeader("Authorization", AuthorizationHeader);
	Request->SetHeader("x-amz-content-sha256", contentHash);
	Request->SetHeader("x-amz-security-token", sessionToken);

	 // LOG_CODE_7019: Authorization header set for S3 request
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7019: Authorization header set."));

	// Execute the request
	Request->OnRequestProgress().BindUObject(this, &UCesiumClient::OnS3UploadProgress);
	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::NotifyCesiumUploadComplete);
	Request->ProcessRequest();
	// LOG_CODE_7020: S3 request execution started
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_7020: S3 request execution started."));
}

void UCesiumClient::NotifyCesiumUploadComplete(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	// LOG_CODE_8001: NotifyCesiumUploadComplete method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_8001: NotifyCesiumUploadComplete method called."));

	// Validate the response from 
	if (!wasSuccessful || !response.IsValid())
	{
		 // LOG_CODE_8002: Failed to upload file to S3
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_8002: Failed to upload file to S3. Invalid or unsuccessful response."));
        return;
	}
	else if (response->GetResponseCode() != 200)
	{
		// LOG_CODE_8003: Non-200 response code received from S3
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_8003: Failed to upload file to S3. Response code: %d Content: %s"), response->GetResponseCode(), *response->GetContentAsString());
        return;
	}

	// LOG_CODE_8004: File successfully uploaded to S3
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_8004: File successfully uploaded to S3."));


	// Notify cesium that the upload is complete here
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
    {
        // LOG_CODE_8005: Failed to retrieve HTTP module for notifying Cesium
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_8005: Failed to retrieve HTTP module for notifying Cesium."));
        return;
    }
// LOG_CODE_8006: HTTP module successfully retrieved for notifying Cesium
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_8006: HTTP module successfully retrieved for notifying Cesium."));


	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(fNotifyCompleteURL);
	Request->SetVerb(fNotifyCompleteVerb);
	 // LOG_CODE_8007: Notify complete request created with URL and Verb
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_8007: Notify complete request created. URL: %s, Verb: %s"), *fNotifyCompleteURL, *fNotifyCompleteVerb);

    // Set authorization header
    Request->SetHeader("Authorization", "Bearer " + this->fCesiumToken);

    // LOG_CODE_8008: Authorization header set for Cesium notification
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_8008: Authorization header set. Token: Bearer %s"), *this->fCesiumToken);

    // Bind the response callback
    Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::OnCesiumUploadCompletion);

    // LOG_CODE_8009: Response callback bound to OnCesiumUploadCompletion
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_8009: Response callback bound to OnCesiumUploadCompletion."));

	Request->ProcessRequest();
}

void UCesiumClient::OnS3UploadProgress(FHttpRequestPtr request, int32 bytesSent, int32 bytesReceived) {
	// LOG_CODE_9001: OnS3UploadProgress method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_9001: OnS3UploadProgress method called."));

	float percentage = 100.f * bytesSent / fFileSize;
	UE_LOG(LogTemp, Log, TEXT("S3 upload progress: %d/%d (%.0f%%)"), bytesSent, fFileSize, percentage);
// LOG_CODE_9004: Bytes received during upload (for monitoring purposes)
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_9004: Bytes received during upload: %d bytes"), bytesReceived);
}

void UCesiumClient::OnCesiumUploadCompletion(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	 // LOG_CODE_10001: OnCesiumUploadCompletion method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_10001: OnCesiumUploadCompletion method called."));

	OnCesiumUploadCompletionResponse.Broadcast();
	  // LOG_CODE_10002: Cesium upload completion broadcast executed
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_10002: Cesium upload completion broadcast executed."));

	if (!wasSuccessful || !response.IsValid())
	{
		 // LOG_CODE_10003: Failed to notify Cesium due to invalid or unsuccessful response
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_10003: Failed to notify Cesium. Invalid or unsuccessful response."));
        return;
	}
	else if (response->GetResponseCode() != 204)
	{
		// LOG_CODE_10004: Non-204 response code from Cesium
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_10004: Failed to notify Cesium. Response code: %d Content: %s"), response->GetResponseCode(), *response->GetContentAsString());
        return;
	}
	// LOG_CODE_10005: Successfully notified Cesium of file upload
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_10005: Successfully notified Cesium of file upload."));
}

void UCesiumClient::ListAssets(bool retreiveFlag)
{
	// LOG_CODE_11001: ListAssets method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_11001: ListAssets method called with retrieveFlag: %d."), retreiveFlag);

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
    {
        // LOG_CODE_11002: Failed to retrieve HTTP module
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_11002: Failed to retrieve HTTP module."));
        return;
    }
	// LOG_CODE_11003: HTTP module successfully retrieved
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_11003: HTTP module successfully retrieved."));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL("https://api.cesium.com/v1/assets");
	Request->SetVerb("GET");	
	// LOG_CODE_11004: HTTP request created for listing assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_11004: HTTP request created. URL: https://api.cesium.com/v1/assets, Verb: GET"));
	Request->SetHeader("Authorization", "Bearer " + fCesiumToken);
	// LOG_CODE_11005: Authorization header set for listing assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_11005: Authorization header set. Token: Bearer %s"), *fCesiumToken);

	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::ListResponse);
	// LOG_CODE_11006: Response callback bound to ListResponse
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_11006: Response callback bound to ListResponse."));

	Request->ProcessRequest();
}

void UCesiumClient::ListResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	// LOG_CODE_12001: ListResponse method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_12001: ListResponse method called."));

	FString data = response->GetContentAsString();
	// LOG_CODE_12004: Displaying HTTP GET response from Cesium
    UE_LOG(LogTemp, Display, TEXT("LOG_CODE_12004: HTTP GET response from Cesium: %s"), *data);
}

void UCesiumClient::RetrieveAllAssets()
{
	 // LOG_CODE_13001: RetrieveAllAssets method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_13001: RetrieveAllAssets method called."));

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
    {
        // LOG_CODE_13002: Failed to retrieve HTTP module
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_13002: Failed to retrieve HTTP module."));
        return;
    }
	// LOG_CODE_13003: HTTP module successfully retrieved
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_13003: HTTP module successfully retrieved."));


	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL("https://api.cesium.com/v1/assets");
	Request->SetVerb("GET");
	// LOG_CODE_13004: HTTP request created for retrieving all assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_13004: HTTP request created. URL: https://api.cesium.com/v1/assets, Verb: GET"));


	Request->SetHeader("Authorization", "Bearer " + fCesiumToken);

	 // LOG_CODE_13005: Authorization header set for retrieving all assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_13005: Authorization header set. Token: Bearer %s"), *fCesiumToken);

	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::StoreAllAssets);
	 // LOG_CODE_13006: Response callback bound to StoreAllAssets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_13006: Response callback bound to StoreAllAssets."));

	Request->ProcessRequest();
	// LOG_CODE_13008: HTTP request successfully executed for retrieving all assets
        UE_LOG(LogTemp, Log, TEXT("LOG_CODE_13008: HTTP request successfully executed for retrieving all assets."));
}

void UCesiumClient::RetrieveActiveAssets()
{
	// LOG_CODE_14001: RetrieveActiveAssets method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_14001: RetrieveActiveAssets method called."));

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
    {
        // LOG_CODE_14002: Failed to retrieve HTTP module
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_14002: Failed to retrieve HTTP module."));
        return;
    }
	// LOG_CODE_14003: HTTP module successfully retrieved
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_14003: HTTP module successfully retrieved."));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL("https://api.cesium.com/v1/assets?search=ACTIVE");
	Request->SetVerb("GET");

	// LOG_CODE_14004: HTTP request created for retrieving active assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_14004: HTTP request created. URL: https://api.cesium.com/v1/assets?search=ACTIVE, Verb: GET"));

    // Set authorization header
    Request->SetHeader("Authorization", "Bearer " + fCesiumToken);

    // LOG_CODE_14005: Authorization header set for retrieving active assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_14005: Authorization header set. Token: Bearer %s"), *fCesiumToken);

    // Bind the response callback to StoreActiveAssets
    Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::StoreActiveAssets);

    // LOG_CODE_14006: Response callback bound to StoreActiveAssets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_14006: Response callback bound to StoreActiveAssets."));

	Request->ProcessRequest();
	 // LOG_CODE_14008: HTTP request successfully executed for retrieving active assets
        UE_LOG(LogTemp, Log, TEXT("LOG_CODE_14008: HTTP request successfully executed for retrieving active assets."));
}

void UCesiumClient::StoreAllAssets(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	// LOG_CODE_15001: StoreAllAssets method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_15001: StoreAllAssets method called."));

	FString data = response->GetContentAsString();
	// LOG_CODE_15004: Displaying HTTP GET response from Cesium
    UE_LOG(LogTemp, Display, TEXT("LOG_CODE_15004: HTTP GET response from Cesium: %s"), *data);

	// Parse the JSON response
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(data);
	TSharedPtr<FJsonObject> jsonObject;

	if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
	{
		// LOG_CODE_15005: Failed to parse JSON from the response
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_15005: Failed to parse JSON from the response."));
        return;
	}	
	// LOG_CODE_15006: JSON parsing successful, processing assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_15006: JSON parsing successful, processing assets."));


	const TArray<TSharedPtr<FJsonValue, ESPMode::ThreadSafe>> items = jsonObject->GetArrayField("items");
	for (int i = 0; i < items.Num(); i++)
	{
		const TSharedPtr<FJsonObject> itemObject = items[i]->AsObject();
		if (itemObject.IsValid())
		{
			UCesiumAsset* lAsset = NewObject<UCesiumAsset>(this);
			lAsset->AddToRoot(); // GC hack
			FString lId;
			FString lName;
			FString lUploadDate;
			FString lDataType;
			FString lDataSize;
			// lDataSize is stored in bytes.
			if (itemObject->TryGetStringField("id", lId) &&
				itemObject->TryGetStringField("name", lName) &&
				itemObject->TryGetStringField("dateAdded", lUploadDate) &&
				itemObject->TryGetStringField("type", lDataType) &&
				itemObject->TryGetStringField("bytes", lDataSize))
			{

				lAsset->Construct(lId, lName, lUploadDate, lDataType, lDataSize);
				// Skip adding assets to the list if they are components of the digital twin engine.
				if (fIgnoredAssets.Contains(lName))
				{
					 // LOG_CODE_15007: Ignoring asset since it's part of the ignored assets list
                    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_15007: Ignoring asset %s since it's part of the ignored assets list."), *lName);
                    continue;
				}
				fAllAssetData.Add(lAsset);
				UE_LOG(LogTemp, Display, TEXT("LOG_CODE_15008: Stored asset %d of retrieved asset data \n id: %s\n name: %s\n dateAdded: %s\n type: %s\n bytes: %s\n"), 
                    fAllAssetData.Num(), *lAsset->GetId(), *lAsset->GetItemName(), *lAsset->GetUploadDate(), *lAsset->GetDataType(), *lAsset->GetDataSize());
            }
		}
	}
	// LOG_CODE_15009: Finished processing and storing all assets
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_15009: Finished processing and storing all assets."));

	RetreiveAllAssetsResponse.Broadcast();
}

void UCesiumClient::UpdateAssetActiveState(UCesiumAsset* aCesiumAsset)
{
	// LOG_CODE_16001: UpdateAssetActiveState method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16001: UpdateAssetActiveState method called for asset: %s"), *aCesiumAsset->fId);

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
    {
        // LOG_CODE_16002: Failed to retrieve HTTP module
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_16002: Failed to retrieve HTTP module."));
        return;
    }
	  if (aCesiumAsset->fId == TEXT(""))
    {
        // LOG_CODE_16003: Asset ID is empty, cannot proceed with updating state
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_16003: Asset ID is empty, cannot proceed with updating state."));
        return;
    }
	// LOG_CODE_16004: HTTP module successfully retrieved
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16004: HTTP module successfully retrieved for updating asset state."));


	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	UE_LOG(LogTemp, Error, TEXT("LOG_CODE_16005: Passed in Asset ID: %s"), *aCesiumAsset->fId);

	Request->SetURL("https://api.cesium.com/v1/assets/" + aCesiumAsset->fId);
	Request->SetVerb("PATCH");

	// LOG_CODE_16006: HTTP request created for updating asset state
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16006: HTTP request created. URL: https://api.cesium.com/v1/assets/%s, Verb: PATCH"), *aCesiumAsset->fId);

	Request->SetHeader("Authorization", "Bearer " + fCesiumToken);
	Request->SetHeader("Content-Type", "application/json");
	// LOG_CODE_16007: Headers set for the request
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16007: Headers set. Authorization: Bearer %s, Content-Type: application/json"), *fCesiumToken);


	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	aCesiumAsset->ToggleCurrentlyActive();
	// LOG_CODE_16008: Toggled the active state of the asset
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16008: Toggled the active state of the asset. New name: %s"), *aCesiumAsset->GetItemName());

	UE_LOG(LogTemp, Error, TEXT("Changing name to: %s"), *aCesiumAsset->GetItemName());
	JsonObject->SetStringField("name", aCesiumAsset->GetItemName());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	// LOG_CODE_16010: JSON payload serialized successfully
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16010: JSON payload serialized successfully: %s"), *RequestBody);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::LogCesiumResponse);
	 // LOG_CODE_16011: Response callback bound to LogCesiumResponse
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16011: Response callback bound to LogCesiumResponse."));

	Request->ProcessRequest();
 // LOG_CODE_16013: HTTP request successfully executed for updating asset state
        UE_LOG(LogTemp, Log, TEXT("LOG_CODE_16013: HTTP request successfully executed for updating asset state."));
}

void UCesiumClient::LogCesiumResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	// LOG_CODE_17001: LogCesiumResponse method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_17001: LogCesiumResponse method called."));

	// Check if the request was successful
    if (!wasSuccessful || !response.IsValid())
    {
        // LOG_CODE_17002: Failed to update asset data on Cesium due to request failure
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_17002: Failed to update asset data on Cesium. Request was unsuccessful or response is invalid."));
        return;
    }
	else if (response->GetResponseCode() != 204)
	{
		// LOG_CODE_17003: Cesium responded with an error code and response content
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_17003: Failed to update asset data on Cesium. Response code: %d, Content: %s"), response->GetResponseCode(), *response->GetContentAsString());
        return;
	}
	// LOG_CODE_17004: Broadcasting UpdateAssetActiveStateResponse delegate
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_17004: Broadcasting UpdateAssetActiveStateResponse delegate."));

	// broadcasting to the delegate on success.
	UpdateAssetActiveStateResponse.Broadcast();
    // LOG_CODE_17005: Successfully updated asset data on Cesium
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_17005: Successfully updated asset data on Cesium."));
}


void UCesiumClient::DeleteAssetFromCesiumIon(UCesiumAsset* aCesiumAsset)
{
	// LOG_CODE_18001: DeleteAssetFromCesiumIon method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_18001: DeleteAssetFromCesiumIon method called for asset: %s"), *aCesiumAsset->fId);

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
    {
        // LOG_CODE_18002: Failed to retrieve HTTP module
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_18002: Failed to retrieve HTTP module."));
        return;
    }
	// LOG_CODE_18003: HTTP module successfully retrieved
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_18003: HTTP module successfully retrieved."));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	 // Log the details of the asset being deleted
    UE_LOG(LogTemp, Error, TEXT("LOG_CODE_18004: Asset to delete:\nID: %s\nName: %s"), *aCesiumAsset->fId, *aCesiumAsset->GetDisplayName());

	Request->SetURL("https://api.cesium.com/v1/assets/" + aCesiumAsset->fId);
	Request->SetVerb("DELETE");
	// LOG_CODE_18005: HTTP request created for deleting asset
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_18005: HTTP request created. URL: https://api.cesium.com/v1/assets/%s, Verb: DELETE"), *aCesiumAsset->fId);

	Request->SetHeader("Authorization", "Bearer " + fCesiumToken);
	// LOG_CODE_18006: Headers set for the delete request
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_18006: Headers set. Authorization: Bearer %s"), *fCesiumToken);

	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::DeleteAssetResponse);
	// LOG_CODE_18007: Response callback bound to DeleteAssetResponse
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_18007: Response callback bound to DeleteAssetResponse."));

	Request->ProcessRequest();
 // LOG_CODE_18009: HTTP request successfully executed for deleting asset
        UE_LOG(LogTemp, Log, TEXT("LOG_CODE_18009: HTTP request successfully executed for deleting asset: %s"), *aCesiumAsset->fId);
}

void UCesiumClient::DeleteAssetResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	// LOG_CODE_19001: DeleteAssetResponse method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_19001: DeleteAssetResponse method called."));

	if (!wasSuccessful || !response.IsValid())
	{
		// LOG_CODE_19002: Failed to delete asset on Cesium due to request failure
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_19002: Failed to delete asset on Cesium. Request was unsuccessful or response is invalid."));
        return;
	}
	else if (response->GetResponseCode() != 204)
	{
		// LOG_CODE_19003: Cesium responded with an error code and response content
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_19003: Failed to delete asset on Cesium. Response code: %d, Content: %s"), response->GetResponseCode(), *response->GetContentAsString());
        return;
	}
	// LOG_CODE_19004: Asset successfully deleted on Cesium
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_19004: Asset successfully deleted on Cesium."));

	DeleteAssetFromCesiumIonResponse.Broadcast();
	 // LOG_CODE_19005: Local asset list cleared after deletion, retrieving updated list from Cesium
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_19005: Local asset list cleared. Retrieving updated list of assets from Cesium."));

	// clear our local list of assets on successful deletion, and retrieve the nw list from cesium
	fAllAssetData.Empty();
	RetrieveAllAssets();
}

void UCesiumClient::StoreActiveAssets(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	// LOG_CODE_20001: StoreActiveAssets method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_20001: StoreActiveAssets method called."));

	FString data = response->GetContentAsString();
	UE_LOG(LogTemp, Display, TEXT("LOG_CODE_20003: HTTP GET response from Cesium: %s"), *data);


	// Parse the JSON response
	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(data);
	TSharedPtr<FJsonObject> jsonObject;

	if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
	{
		// LOG_CODE_20004: Failed to parse JSON response from Cesium
        UE_LOG(LogTemp, Error, TEXT("LOG_CODE_20004: Failed to parse JSON response from Cesium."));
        return;
	}

	const TArray<TSharedPtr<FJsonValue, ESPMode::ThreadSafe>> items = jsonObject->GetArrayField("items");
	int lActiveTifIndex = 0;
	int lActiveLasIndex = 0;
	for (int i = 0; i < items.Num(); i++)
	{
		const TSharedPtr<FJsonObject> itemObject = items[i]->AsObject();
		if (itemObject.IsValid())
		{
			FString lOutput;
			FString lType;
			if (itemObject->TryGetStringField("id", lOutput))
			{
				itemObject->TryGetStringField("type", lType);
				if (lType == "IMAGERY")
				{
					fActiveTif.Add(lOutput);
					// LOG_CODE_20006: GeoTiff item added
                    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_20006: GeoTiff item added with ID: %s"), *fActiveTif[lActiveTifIndex]);
                    lActiveTifIndex++;
				}
				else if (lType == "3DTILES")
				{
					fActiveLas.Add(lOutput);
					// LOG_CODE_20007: LAS item added
                    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_20007: LAS item added with ID: %s"), *fActiveLas[lActiveLasIndex]);
                    lActiveLasIndex++;
				}
			}
			else
			{
				// LOG_CODE_20008: Active item missing 'id' field
                UE_LOG(LogTemp, Warning, TEXT("LOG_CODE_20008: Active item no. %d does not contain an 'id' field."), i);
            }
		}
	}
	RetrieveActiveAssetsResponse.Broadcast();
}

UCesiumAsset* UCesiumClient::GetAllAssetDataElementByID(FString aId)
{
	// LOG_CODE_30001: GetAllAssetDataElementByID method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_30001: GetAllAssetDataElementByID method called with aId: %s"), *aId);

    // LOG_CODE_30002: Log the number of assets in the list
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_30002: Number of assets in fAllAssetData: %d"), fAllAssetData.Num());

	for (UCesiumAsset* lAsset : fAllAssetData)
	{
		// LOG_CODE_30003: Comparing asset ID with the provided aId
        UE_LOG(LogTemp, Log, TEXT("LOG_CODE_30003: Comparing asset ID: %s with provided aId: %s"), *lAsset->fId, *aId);

        if (lAsset->fId == aId)
		{
			// LOG_CODE_30004: Match found for asset ID
            UE_LOG(LogTemp, Log, TEXT("LOG_CODE_30004: Match found for asset ID: %s"), *lAsset->fId);
            return lAsset;
		}
	}
	// LOG_CODE_30005: No matching asset ID found in the list
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_30005: No match found for asset ID: %s in GetAllAssetDataElementByID."), *aId);


	return NewObject<UCesiumAsset>();
}

TArray<UCesiumAsset*> UCesiumClient::GetAllAssetData() { UE_LOG(LogTemp, Log, TEXT("fAllAssetData Length: %d"), fAllAssetData.Num());	return fAllAssetData; }

int32 UCesiumClient::GetAllAssetSize() { UE_LOG(LogTemp, Log, TEXT("fAllAssetData Length: %d"), fAllAssetData.Num());	return fAllAssetData.Num(); }

TArray<FString> UCesiumClient::GetActiveTif() { UE_LOG(LogTemp, Log, TEXT("fActiveTif Length: %d"), fActiveTif.Num());	return fActiveTif; }

TArray<FString> UCesiumClient::GetActiveLas() { UE_LOG(LogTemp, Log, TEXT("fActiveLas Length: %d"), fActiveLas.Num());	return fActiveLas; }

FString UCesiumClient::GetCesiumToken() { return fCesiumToken; }

