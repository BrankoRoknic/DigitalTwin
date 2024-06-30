﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <aws/iam/IAM_EXPORTS.h>
#include <aws/iam/IAMRequest.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <utility>

namespace Aws
{
namespace IAM
{
namespace Model
{

  /**
   */
  class GetServiceLastAccessedDetailsWithEntitiesRequest : public IAMRequest
  {
  public:
    AWS_IAM_API GetServiceLastAccessedDetailsWithEntitiesRequest();

    // Service request name is the Operation name which will send this request out,
    // each operation should has unique request name, so that we can get operation's name from this request.
    // Note: this is not true for response, multiple operations may have the same response name,
    // so we can not get operation's name from response.
    inline virtual const char* GetServiceRequestName() const override { return "GetServiceLastAccessedDetailsWithEntities"; }

    AWS_IAM_API Aws::String SerializePayload() const override;

  protected:
    AWS_IAM_API void DumpBodyToUrl(Aws::Http::URI& uri ) const override;

  public:

    ///@{
    /**
     * <p>The ID of the request generated by the
     * <code>GenerateServiceLastAccessedDetails</code> operation.</p>
     */
    inline const Aws::String& GetJobId() const{ return m_jobId; }
    inline bool JobIdHasBeenSet() const { return m_jobIdHasBeenSet; }
    inline void SetJobId(const Aws::String& value) { m_jobIdHasBeenSet = true; m_jobId = value; }
    inline void SetJobId(Aws::String&& value) { m_jobIdHasBeenSet = true; m_jobId = std::move(value); }
    inline void SetJobId(const char* value) { m_jobIdHasBeenSet = true; m_jobId.assign(value); }
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithJobId(const Aws::String& value) { SetJobId(value); return *this;}
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithJobId(Aws::String&& value) { SetJobId(std::move(value)); return *this;}
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithJobId(const char* value) { SetJobId(value); return *this;}
    ///@}

    ///@{
    /**
     * <p>The service namespace for an Amazon Web Services service. Provide the service
     * namespace to learn when the IAM entity last attempted to access the specified
     * service.</p> <p>To learn the service namespace for a service, see <a
     * href="https://docs.aws.amazon.com/service-authorization/latest/reference/reference_policies_actions-resources-contextkeys.html">Actions,
     * resources, and condition keys for Amazon Web Services services</a> in the <i>IAM
     * User Guide</i>. Choose the name of the service to view details for that service.
     * In the first paragraph, find the service prefix. For example, <code>(service
     * prefix: a4b)</code>. For more information about service namespaces, see <a
     * href="https://docs.aws.amazon.com/general/latest/gr/aws-arns-and-namespaces.html#genref-aws-service-namespaces">Amazon
     * Web Services service namespaces</a> in the <i>Amazon Web Services General
     * Reference</i>.</p>
     */
    inline const Aws::String& GetServiceNamespace() const{ return m_serviceNamespace; }
    inline bool ServiceNamespaceHasBeenSet() const { return m_serviceNamespaceHasBeenSet; }
    inline void SetServiceNamespace(const Aws::String& value) { m_serviceNamespaceHasBeenSet = true; m_serviceNamespace = value; }
    inline void SetServiceNamespace(Aws::String&& value) { m_serviceNamespaceHasBeenSet = true; m_serviceNamespace = std::move(value); }
    inline void SetServiceNamespace(const char* value) { m_serviceNamespaceHasBeenSet = true; m_serviceNamespace.assign(value); }
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithServiceNamespace(const Aws::String& value) { SetServiceNamespace(value); return *this;}
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithServiceNamespace(Aws::String&& value) { SetServiceNamespace(std::move(value)); return *this;}
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithServiceNamespace(const char* value) { SetServiceNamespace(value); return *this;}
    ///@}

    ///@{
    /**
     * <p>Use this only when paginating results to indicate the maximum number of items
     * you want in the response. If additional items exist beyond the maximum you
     * specify, the <code>IsTruncated</code> response element is <code>true</code>.</p>
     * <p>If you do not include this parameter, the number of items defaults to 100.
     * Note that IAM might return fewer results, even when there are more results
     * available. In that case, the <code>IsTruncated</code> response element returns
     * <code>true</code>, and <code>Marker</code> contains a value to include in the
     * subsequent call that tells the service where to continue from.</p>
     */
    inline int GetMaxItems() const{ return m_maxItems; }
    inline bool MaxItemsHasBeenSet() const { return m_maxItemsHasBeenSet; }
    inline void SetMaxItems(int value) { m_maxItemsHasBeenSet = true; m_maxItems = value; }
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithMaxItems(int value) { SetMaxItems(value); return *this;}
    ///@}

    ///@{
    /**
     * <p>Use this parameter only when paginating results and only after you receive a
     * response indicating that the results are truncated. Set it to the value of the
     * <code>Marker</code> element in the response that you received to indicate where
     * the next call should start.</p>
     */
    inline const Aws::String& GetMarker() const{ return m_marker; }
    inline bool MarkerHasBeenSet() const { return m_markerHasBeenSet; }
    inline void SetMarker(const Aws::String& value) { m_markerHasBeenSet = true; m_marker = value; }
    inline void SetMarker(Aws::String&& value) { m_markerHasBeenSet = true; m_marker = std::move(value); }
    inline void SetMarker(const char* value) { m_markerHasBeenSet = true; m_marker.assign(value); }
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithMarker(const Aws::String& value) { SetMarker(value); return *this;}
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithMarker(Aws::String&& value) { SetMarker(std::move(value)); return *this;}
    inline GetServiceLastAccessedDetailsWithEntitiesRequest& WithMarker(const char* value) { SetMarker(value); return *this;}
    ///@}
  private:

    Aws::String m_jobId;
    bool m_jobIdHasBeenSet = false;

    Aws::String m_serviceNamespace;
    bool m_serviceNamespaceHasBeenSet = false;

    int m_maxItems;
    bool m_maxItemsHasBeenSet = false;

    Aws::String m_marker;
    bool m_markerHasBeenSet = false;
  };

} // namespace Model
} // namespace IAM
} // namespace Aws