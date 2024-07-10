﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <aws/iam/IAM_EXPORTS.h>
#include <aws/iam/IAMRequest.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/iam/model/SortKeyType.h>
#include <utility>

namespace Aws
{
namespace IAM
{
namespace Model
{

  /**
   */
  class GetOrganizationsAccessReportRequest : public IAMRequest
  {
  public:
    AWS_IAM_API GetOrganizationsAccessReportRequest();

    // Service request name is the Operation name which will send this request out,
    // each operation should has unique request name, so that we can get operation's name from this request.
    // Note: this is not true for response, multiple operations may have the same response name,
    // so we can not get operation's name from response.
    inline virtual const char* GetServiceRequestName() const override { return "GetOrganizationsAccessReport"; }

    AWS_IAM_API Aws::String SerializePayload() const override;

  protected:
    AWS_IAM_API void DumpBodyToUrl(Aws::Http::URI& uri ) const override;

  public:

    ///@{
    /**
     * <p>The identifier of the request generated by the
     * <a>GenerateOrganizationsAccessReport</a> operation.</p>
     */
    inline const Aws::String& GetJobId() const{ return m_jobId; }
    inline bool JobIdHasBeenSet() const { return m_jobIdHasBeenSet; }
    inline void SetJobId(const Aws::String& value) { m_jobIdHasBeenSet = true; m_jobId = value; }
    inline void SetJobId(Aws::String&& value) { m_jobIdHasBeenSet = true; m_jobId = std::move(value); }
    inline void SetJobId(const char* value) { m_jobIdHasBeenSet = true; m_jobId.assign(value); }
    inline GetOrganizationsAccessReportRequest& WithJobId(const Aws::String& value) { SetJobId(value); return *this;}
    inline GetOrganizationsAccessReportRequest& WithJobId(Aws::String&& value) { SetJobId(std::move(value)); return *this;}
    inline GetOrganizationsAccessReportRequest& WithJobId(const char* value) { SetJobId(value); return *this;}
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
    inline GetOrganizationsAccessReportRequest& WithMaxItems(int value) { SetMaxItems(value); return *this;}
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
    inline GetOrganizationsAccessReportRequest& WithMarker(const Aws::String& value) { SetMarker(value); return *this;}
    inline GetOrganizationsAccessReportRequest& WithMarker(Aws::String&& value) { SetMarker(std::move(value)); return *this;}
    inline GetOrganizationsAccessReportRequest& WithMarker(const char* value) { SetMarker(value); return *this;}
    ///@}

    ///@{
    /**
     * <p>The key that is used to sort the results. If you choose the namespace key,
     * the results are returned in alphabetical order. If you choose the time key, the
     * results are sorted numerically by the date and time.</p>
     */
    inline const SortKeyType& GetSortKey() const{ return m_sortKey; }
    inline bool SortKeyHasBeenSet() const { return m_sortKeyHasBeenSet; }
    inline void SetSortKey(const SortKeyType& value) { m_sortKeyHasBeenSet = true; m_sortKey = value; }
    inline void SetSortKey(SortKeyType&& value) { m_sortKeyHasBeenSet = true; m_sortKey = std::move(value); }
    inline GetOrganizationsAccessReportRequest& WithSortKey(const SortKeyType& value) { SetSortKey(value); return *this;}
    inline GetOrganizationsAccessReportRequest& WithSortKey(SortKeyType&& value) { SetSortKey(std::move(value)); return *this;}
    ///@}
  private:

    Aws::String m_jobId;
    bool m_jobIdHasBeenSet = false;

    int m_maxItems;
    bool m_maxItemsHasBeenSet = false;

    Aws::String m_marker;
    bool m_markerHasBeenSet = false;

    SortKeyType m_sortKey;
    bool m_sortKeyHasBeenSet = false;
  };

} // namespace Model
} // namespace IAM
} // namespace Aws
