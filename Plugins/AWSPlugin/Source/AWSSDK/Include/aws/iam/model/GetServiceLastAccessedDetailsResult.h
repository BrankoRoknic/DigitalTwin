﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <aws/iam/IAM_EXPORTS.h>
#include <aws/iam/model/JobStatusType.h>
#include <aws/iam/model/AccessAdvisorUsageGranularityType.h>
#include <aws/core/utils/DateTime.h>
#include <aws/core/utils/memory/stl/AWSVector.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/iam/model/ErrorDetails.h>
#include <aws/iam/model/ResponseMetadata.h>
#include <aws/iam/model/ServiceLastAccessed.h>
#include <utility>

namespace Aws
{
template<typename RESULT_TYPE>
class AmazonWebServiceResult;

namespace Utils
{
namespace Xml
{
  class XmlDocument;
} // namespace Xml
} // namespace Utils
namespace IAM
{
namespace Model
{
  class GetServiceLastAccessedDetailsResult
  {
  public:
    AWS_IAM_API GetServiceLastAccessedDetailsResult();
    AWS_IAM_API GetServiceLastAccessedDetailsResult(const Aws::AmazonWebServiceResult<Aws::Utils::Xml::XmlDocument>& result);
    AWS_IAM_API GetServiceLastAccessedDetailsResult& operator=(const Aws::AmazonWebServiceResult<Aws::Utils::Xml::XmlDocument>& result);


    ///@{
    /**
     * <p>The status of the job.</p>
     */
    inline const JobStatusType& GetJobStatus() const{ return m_jobStatus; }
    inline void SetJobStatus(const JobStatusType& value) { m_jobStatus = value; }
    inline void SetJobStatus(JobStatusType&& value) { m_jobStatus = std::move(value); }
    inline GetServiceLastAccessedDetailsResult& WithJobStatus(const JobStatusType& value) { SetJobStatus(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithJobStatus(JobStatusType&& value) { SetJobStatus(std::move(value)); return *this;}
    ///@}

    ///@{
    /**
     * <p>The type of job. Service jobs return information about when each service was
     * last accessed. Action jobs also include information about when tracked actions
     * within the service were last accessed.</p>
     */
    inline const AccessAdvisorUsageGranularityType& GetJobType() const{ return m_jobType; }
    inline void SetJobType(const AccessAdvisorUsageGranularityType& value) { m_jobType = value; }
    inline void SetJobType(AccessAdvisorUsageGranularityType&& value) { m_jobType = std::move(value); }
    inline GetServiceLastAccessedDetailsResult& WithJobType(const AccessAdvisorUsageGranularityType& value) { SetJobType(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithJobType(AccessAdvisorUsageGranularityType&& value) { SetJobType(std::move(value)); return *this;}
    ///@}

    ///@{
    /**
     * <p>The date and time, in <a href="http://www.iso.org/iso/iso8601">ISO 8601
     * date-time format</a>, when the report job was created.</p>
     */
    inline const Aws::Utils::DateTime& GetJobCreationDate() const{ return m_jobCreationDate; }
    inline void SetJobCreationDate(const Aws::Utils::DateTime& value) { m_jobCreationDate = value; }
    inline void SetJobCreationDate(Aws::Utils::DateTime&& value) { m_jobCreationDate = std::move(value); }
    inline GetServiceLastAccessedDetailsResult& WithJobCreationDate(const Aws::Utils::DateTime& value) { SetJobCreationDate(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithJobCreationDate(Aws::Utils::DateTime&& value) { SetJobCreationDate(std::move(value)); return *this;}
    ///@}

    ///@{
    /**
     * <p> A <code>ServiceLastAccessed</code> object that contains details about the
     * most recent attempt to access the service.</p>
     */
    inline const Aws::Vector<ServiceLastAccessed>& GetServicesLastAccessed() const{ return m_servicesLastAccessed; }
    inline void SetServicesLastAccessed(const Aws::Vector<ServiceLastAccessed>& value) { m_servicesLastAccessed = value; }
    inline void SetServicesLastAccessed(Aws::Vector<ServiceLastAccessed>&& value) { m_servicesLastAccessed = std::move(value); }
    inline GetServiceLastAccessedDetailsResult& WithServicesLastAccessed(const Aws::Vector<ServiceLastAccessed>& value) { SetServicesLastAccessed(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithServicesLastAccessed(Aws::Vector<ServiceLastAccessed>&& value) { SetServicesLastAccessed(std::move(value)); return *this;}
    inline GetServiceLastAccessedDetailsResult& AddServicesLastAccessed(const ServiceLastAccessed& value) { m_servicesLastAccessed.push_back(value); return *this; }
    inline GetServiceLastAccessedDetailsResult& AddServicesLastAccessed(ServiceLastAccessed&& value) { m_servicesLastAccessed.push_back(std::move(value)); return *this; }
    ///@}

    ///@{
    /**
     * <p>The date and time, in <a href="http://www.iso.org/iso/iso8601">ISO 8601
     * date-time format</a>, when the generated report job was completed or failed.</p>
     * <p>This field is null if the job is still in progress, as indicated by a job
     * status value of <code>IN_PROGRESS</code>.</p>
     */
    inline const Aws::Utils::DateTime& GetJobCompletionDate() const{ return m_jobCompletionDate; }
    inline void SetJobCompletionDate(const Aws::Utils::DateTime& value) { m_jobCompletionDate = value; }
    inline void SetJobCompletionDate(Aws::Utils::DateTime&& value) { m_jobCompletionDate = std::move(value); }
    inline GetServiceLastAccessedDetailsResult& WithJobCompletionDate(const Aws::Utils::DateTime& value) { SetJobCompletionDate(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithJobCompletionDate(Aws::Utils::DateTime&& value) { SetJobCompletionDate(std::move(value)); return *this;}
    ///@}

    ///@{
    /**
     * <p>A flag that indicates whether there are more items to return. If your results
     * were truncated, you can make a subsequent pagination request using the
     * <code>Marker</code> request parameter to retrieve more items. Note that IAM
     * might return fewer than the <code>MaxItems</code> number of results even when
     * there are more results available. We recommend that you check
     * <code>IsTruncated</code> after every call to ensure that you receive all your
     * results.</p>
     */
    inline bool GetIsTruncated() const{ return m_isTruncated; }
    inline void SetIsTruncated(bool value) { m_isTruncated = value; }
    inline GetServiceLastAccessedDetailsResult& WithIsTruncated(bool value) { SetIsTruncated(value); return *this;}
    ///@}

    ///@{
    /**
     * <p>When <code>IsTruncated</code> is <code>true</code>, this element is present
     * and contains the value to use for the <code>Marker</code> parameter in a
     * subsequent pagination request.</p>
     */
    inline const Aws::String& GetMarker() const{ return m_marker; }
    inline void SetMarker(const Aws::String& value) { m_marker = value; }
    inline void SetMarker(Aws::String&& value) { m_marker = std::move(value); }
    inline void SetMarker(const char* value) { m_marker.assign(value); }
    inline GetServiceLastAccessedDetailsResult& WithMarker(const Aws::String& value) { SetMarker(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithMarker(Aws::String&& value) { SetMarker(std::move(value)); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithMarker(const char* value) { SetMarker(value); return *this;}
    ///@}

    ///@{
    /**
     * <p>An object that contains details about the reason the operation failed.</p>
     */
    inline const ErrorDetails& GetError() const{ return m_error; }
    inline void SetError(const ErrorDetails& value) { m_error = value; }
    inline void SetError(ErrorDetails&& value) { m_error = std::move(value); }
    inline GetServiceLastAccessedDetailsResult& WithError(const ErrorDetails& value) { SetError(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithError(ErrorDetails&& value) { SetError(std::move(value)); return *this;}
    ///@}

    ///@{
    
    inline const ResponseMetadata& GetResponseMetadata() const{ return m_responseMetadata; }
    inline void SetResponseMetadata(const ResponseMetadata& value) { m_responseMetadata = value; }
    inline void SetResponseMetadata(ResponseMetadata&& value) { m_responseMetadata = std::move(value); }
    inline GetServiceLastAccessedDetailsResult& WithResponseMetadata(const ResponseMetadata& value) { SetResponseMetadata(value); return *this;}
    inline GetServiceLastAccessedDetailsResult& WithResponseMetadata(ResponseMetadata&& value) { SetResponseMetadata(std::move(value)); return *this;}
    ///@}
  private:

    JobStatusType m_jobStatus;

    AccessAdvisorUsageGranularityType m_jobType;

    Aws::Utils::DateTime m_jobCreationDate;

    Aws::Vector<ServiceLastAccessed> m_servicesLastAccessed;

    Aws::Utils::DateTime m_jobCompletionDate;

    bool m_isTruncated;

    Aws::String m_marker;

    ErrorDetails m_error;

    ResponseMetadata m_responseMetadata;
  };

} // namespace Model
} // namespace IAM
} // namespace Aws
