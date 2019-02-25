#include "Authorization.h"

namespace Onvif
{

void AuthorizationHolder::updateAuthorization(const AuthorizationInfo& info)
{
	std::lock_guard<std::mutex> lock(m_authGuard);
	m_authInfo = info;
}

Onvif::AuthorizationInfo AuthorizationHolder::getAuthorization() const
{
	std::lock_guard<std::mutex> lock(m_authGuard);
	return m_authInfo;
}

void AuthorizationHolder::resetAuthentication()
{
	std::lock_guard<std::mutex> lock(m_authGuard);
	m_authInfo = AuthorizationInfo();
}

namespace
{

void safeDeleteStringPointer(char** pointer)
{
	if (pointer && *pointer)
	{
		free(*pointer);
		*pointer = 0;
	}
}

char* safeDuplicateString(const char* source)
{
	return soap_strdup(0, const_cast<char*>(source));
}

struct http_da_info duplicateDigestInfo(const http_da_info& info)
{
	http_da_info resultInfo = {};
	
	resultInfo.alg = safeDuplicateString(info.alg);
	resultInfo.authrealm = safeDuplicateString(info.authrealm);
	resultInfo.nonce = safeDuplicateString(info.nonce);
	resultInfo.opaque = safeDuplicateString(info.opaque);
	resultInfo.passwd = safeDuplicateString(info.passwd);
	resultInfo.qop = safeDuplicateString(info.qop);
	resultInfo.userid = safeDuplicateString(info.userid);

	return resultInfo;
}

}

void safeReleaseDigestInfo(http_da_info& info)
{
	safeDeleteStringPointer(&info.alg);
	safeDeleteStringPointer(&info.authrealm);
	safeDeleteStringPointer(&info.nonce);
	safeDeleteStringPointer(&info.opaque);
	safeDeleteStringPointer(&info.passwd);
	safeDeleteStringPointer(&info.qop);
	safeDeleteStringPointer(&info.userid);
}

HttpDigestInfo::HttpDigestInfo() :
	m_digestInfo()
{
}

HttpDigestInfo::HttpDigestInfo(const http_da_info& info) :
	m_digestInfo(duplicateDigestInfo(info))
{
}

HttpDigestInfo::HttpDigestInfo(const HttpDigestInfo& info) :
	m_digestInfo(duplicateDigestInfo(info.m_digestInfo))
{

}

HttpDigestInfo::~HttpDigestInfo()
{
	safeReleaseDigestInfo(m_digestInfo);
}

HttpDigestInfo& HttpDigestInfo::operator=(const HttpDigestInfo& info)
{
	return this->operator=(info.m_digestInfo);
}

HttpDigestInfo& HttpDigestInfo::operator=(const http_da_info& info)
{
	safeReleaseDigestInfo(m_digestInfo);
	m_digestInfo = duplicateDigestInfo(info);
	return *this;

}

http_da_info HttpDigestInfo::duplicate() const
{
	return duplicateDigestInfo(m_digestInfo);
}

}
