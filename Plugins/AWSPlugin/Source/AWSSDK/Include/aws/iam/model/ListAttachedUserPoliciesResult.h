﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <aws/iam/IAM_EXPORTS.h>
#include <aws/core/utils/memory/stl/AWSVector.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/iam/model/ResponseMetadata.h>
#include <aws/iam/model/AttachedPolicy.h>
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
  /**
   * <p>Contains the response to a successful <a>ListAttachedUserPolicies</a>
   * request. </p><p><h3>See Also:</h3>   <a
   * href="http://docs.aws.amazon.com/goto/WebAPI/iam-2010-05-08/ListAttachedUserPoliciesResponse">AWS
   * API Reference</a></p>
   */
  class ListAttachedUserPoliciesResult
  {
  public:
    AWS_IAM_API ListAttachedUserPoliciesResult();
    AWS_IAM_API ListAttachedUserPoliciesResult(const Aws::AmazonWebServiceResult<Aws::Utils::Xml::XmlDocument>& result);
    AWS_IAM_API ListAttachedUserPoliciesResult& operator=(const Aws::AmazonWebServiceResult<Aws::Utils::Xml::XmlDocument>& result);


    ///@{
    /**
     * <p>A list of the attached policies.</p>
     */
    inline const Aws::Vector<AttachedPolicy>& GetAttachedPolicies() const{ return m_attachedPolicies; }
    inline void SetAttachedPolicies(const Aws::Vector<AttachedPolicy>& value) { m_attachedPolicies = value; }
    inline void SetAttachedPolicies(Aws::Vector<AttachedPolicy>&& value) { m_attachedPolicies = std::move(value); }
    inline ListAttachedUserPoliciesResult& WithAttachedPolicies(const Aws::Vector<AttachedPolicy>& value) { SetAttachedPolicies(value); return *this;}
    inline ListAttachedUserPoliciesResult& WithAttachedPolicies(Aws::Vector<AttachedPolicy>&& value) { SetAttachedPolicies(std::move(value)); return *this;}
    inline ListAttachedUserPoliciesResult& AddAttachedPolicies(const AttachedPolicy& value) { m_attachedPolicies.push_back(value); return *this; }
    inline ListAttachedUserPoliciesResult& AddAttachedPolicies(AttachedPolicy&& value) { m_attachedPolicies.push_back(std::move(value)); return *this; }
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
    inline ListAttachedUserPoliciesResult& WithIsTruncated(bool value) { SetIsTruncated(value); return *this;}
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
    inline ListAttachedUserPoliciesResult& WithMarker(const Aws::String& value) { SetMarker(value); return *this;}
    inline ListAttachedUserPoliciesResult& WithMarker(Aws::String&& value) { SetMarker(std::move(value)); return *this;}
    inline ListAttachedUserPoliciesResult& WithMarker(const char* value) { SetMarker(value); return *this;}
    ///@}

    ///@{
    
    inline const ResponseMetadata& GetResponseMetadata() const{ return m_responseMetadata; }
    inline void SetResponseMetadata(const ResponseMetadata& value) { m_responseMetadata = value; }
    inline void SetResponseMetadata(ResponseMetadata&& value) { m_responseMetadata = std::move(value); }
    inline ListAttachedUserPoliciesResult& WithResponseMetadata(const ResponseMetadata& value) { SetResponseMetadata(value); return *this;}
    inline ListAttachedUserPoliciesResult& WithResponseMetadata(ResponseMetadata&& value) { SetResponseMetadata(std::move(value)); return *this;}
    ///@}
  private:

    Aws::Vector<AttachedPolicy> m_attachedPolicies;

    bool m_isTruncated;

    Aws::String m_marker;

    ResponseMetadata m_responseMetadata;
  };

} // namespace Model
} // namespace IAM
} // namespace Aws
