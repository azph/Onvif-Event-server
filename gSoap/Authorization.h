#ifndef ONVIF_AUTHORIZATION_H
#define ONVIF_AUTHORIZATION_H

#include <gSoap/httpda.h>
#include <mutex>

namespace Onvif
{

enum AuthState
{
	AUTH_NOT_USED,
	AUTH_USER_TOKEN_TEXT,
	AUTH_USER_TOKEN_DIGEST,
	AUTH_DIGEST,
};

void safeReleaseDigestInfo(http_da_info& info);

class HttpDigestInfo
{
public:
	HttpDigestInfo();
	HttpDigestInfo(const http_da_info& info);
	HttpDigestInfo(const HttpDigestInfo& info);
	~HttpDigestInfo();

public:
	HttpDigestInfo& operator=(const HttpDigestInfo& info);
	HttpDigestInfo& operator=(const http_da_info& info);

	http_da_info duplicate() const;

private:
	http_da_info			m_digestInfo;
};

class AuthorizationInfo
{
public:
	AuthorizationInfo() :
		m_authScheme(AUTH_NOT_USED)
	{}

	AuthorizationInfo(const http_da_info& info, AuthState authScheme) :
		m_digestInfo(info),
		m_authScheme(authScheme)
	{}

	HttpDigestInfo				m_digestInfo;
	AuthState					m_authScheme;
};


class AuthorizationHolder
{
public:
	AuthorizationInfo getAuthorization() const;
	void updateAuthorization(const AuthorizationInfo& info);
	void resetAuthentication();

private:
	AuthorizationInfo			m_authInfo;
	mutable std::mutex			m_authGuard;
};

}

#endif

