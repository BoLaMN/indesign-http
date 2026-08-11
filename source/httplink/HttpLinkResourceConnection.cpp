//========================================================================================
//
//  $File$
//
//  Owner: Adobe Developer Technologies
//
//  $Author$
//
//  $DateTime$
//
//  $Revision$
//
//  $Change$
//
//  ADOBE CONFIDENTIAL
//  
//  Copyright 2019 Adobe
//  All Rights Reserved.
//
//  NOTICE: Adobe permits you to use, modify, and distribute this file in
//  accordance with the terms of the Adobe license agreement accompanying
//  it. If you have received this file from a source other than Adobe,
//  then your use, modification, or distribution of it requires the prior
//  written permission of Adobe. 
//
//========================================================================================

#include "VCPlugInHeaders.h"

#include "CPMUnknown.h"
#include "HttpLinkID.h"

// Interfaces
#include "IHTTPLinkResourceConnection.h"
#include "IHTTPLinkManager.h"

#include "IJsonUtils.h"

/**
     Provide methods to authorize connection to server
 
     @ingroup httplink.sdk
 */

class HttpLnkLinkResourceConnection : public CPMUnknown<IHTTPLinkResourceConnection>
{
public:
    typedef CPMUnknown<IHTTPLinkResourceConnection> inherited;

    /**
     Constructor
     */
    HttpLnkLinkResourceConnection(IPMUnknown* boss);
    
    /**
     Destructor
     */
    ~HttpLnkLinkResourceConnection();

    /**
     Shuts down the current connection
     @return kTrue if the Http Connection is shutdown else kFalse
     */
    virtual bool16 Shutdown();

    /**
     Checks if the current connection is connected
     @return kTrue if the Http Connection is connected else kFalse
     */
    virtual bool16 IsConnected() const;

    /**
     Checks if the current connection is authorized
     @return kTrue if the Http Connection is authorized else kFalse.
     */
    virtual bool16 Connect();

    /**
     Creates/Logs-in the current connection
     @return kTrue if connection is created/logged-in successfully else kFalse.
     */
    virtual bool16 IsAuthorized() const;

    /**
     Reconnects the current connection
     @return kTrue if re-connection is successful else kFalse
     */
    virtual bool16 Reconnect();

    /**
     Disconnects the current connection
     @return kTrue if the connection is diconnected else kFalse
     */
    virtual bool16 Disconnect();

    /**
     Fetches the connection ID for the current connection
     @return ConnectionId for the current connection
     */
    virtual IHTTPLinkResourceConnection::ConnectionId GetConnectionId();

    
    /**
     Gets the login credentials for the given connection
     @param loginInformation        [OUT]  The login information that requires to be set for the given connection
     @return kFalse if credentials could not be encoded, else kTrue
     */
    virtual bool16 GetLoginCredentials(std::stringstream& loginInformation) const;

    /**
     Sets the login credentials for the given connection
     @param loginInformation        [IN]  The login information that requires to be set for the given connection
     @return kTrue if credentials are set
     */
    virtual bool16 SetLoginCredentials(const std::stringstream& loginInformation);

	/**
	** For internal purpose only
	*/
	virtual void ReadWriteSnapshotForBackgroundThread(IPMStream* s)
	{
        XferStdString(s, fLoginInfo.fToken);
        XferStdString(s, fLoginInfo.fUsername);
        XferStdString(s, fLoginInfo.fPassword);
	}
    
private:
    struct LoginInfo
    {
        std::string fToken;
        std::string fUsername;
        std::string fPassword;
    };
    
    // LoginInfo represents the login information
    // between client and server
    LoginInfo fLoginInfo;
    
    void XferStdString(IPMStream* s, std::string& str)
    {
        PMString pmString;
        if (s->IsReading())
        {
            pmString.ReadWrite(s);
            str = pmString.GetUTF8String();
        }
        else
        {
            pmString.SetUTF8String(str);
            pmString.ReadWrite(s);
        }
    }
    
    // Private Utility functions
    
    /**
     * Generates the session token using the API wrapper
     * @param token[OUT] The login token which is generated
     * @return true if token is generated successfully, otherwise false
     */
    bool16 GenerateToken(std::string &token);
    
    /**
     * Converts LoginInfo to JSON
     * @param loginInfo [IN] input logininfo object
     * @param json[OUT] output json object
     */
    static void convertLoginInfoToJson(LoginInfo loginInfo, JSON& json);
    
    /**
     * Converts json object to LoginInfo object
     * @param loginInfo [OUT] output login info object
     * @param json [IN] input json object
     */
    static bool16 convertJsonToLoginInfo(LoginInfo &loginInfo, JSON& json);
};

CREATE_PMINTERFACE(HttpLnkLinkResourceConnection, kHttpLnkLinkResourceConnectionImpl)


/* Constructor
*/
HttpLnkLinkResourceConnection::HttpLnkLinkResourceConnection(IPMUnknown* boss)
: inherited(boss)
{
}

/* Destructor
 */
HttpLnkLinkResourceConnection::~HttpLnkLinkResourceConnection()
{
}

/* Shutdown
 */
bool16 HttpLnkLinkResourceConnection::Shutdown()
{
    // Clear the token in case connection is shutdown
    fLoginInfo.fToken = "";
    return kTrue;
}

/* IsConnected

   Anonymous HTTP is sessionless: there is no handshake to perform and nothing
   to keep alive, so we are always in a position to issue a request. Reporting
   otherwise is not a harmless conservatism -- the framework treats a
   disconnected connection as fatal:

     * HTTPAssetLinkResourceStateUpdater::UpdateLinkResourceState forces the
       resource to kInaccessible, which is the "?" badge in the Links panel;
     * HTTPAssetLinkResourceHandler refuses to serve the asset at all.

   That is what made links come back unresolved after a document was closed and
   reopened: nothing had called Connect() in the new session, so every link was
   marked inaccessible before a single request was attempted.

   A server that actually rejects us still gets the last word -- the status probe
   sees the 401/403 and reports the link inaccessible on the evidence.
 */
bool16 HttpLnkLinkResourceConnection::IsConnected() const
{
    return kTrue;
}

/* Connect
 */
bool16 HttpLnkLinkResourceConnection::Connect()
{
    // Anonymous connections are first-class: a plain public URL requires no
    // credentials. GenerateToken picks up a bearer token when one was
    // configured and succeeds without one when it wasn't.
    GenerateToken(fLoginInfo.fToken);
    return kTrue;
}

/* IsAuthorized
 */
bool16 HttpLnkLinkResourceConnection::IsAuthorized() const
{
    // Nothing to authorize against for anonymous HTTP; a server that disagrees
    // will answer 401/403 and the status probe reports the link inaccessible.
	return kTrue;
}

/* Reconnect
 */
bool16 HttpLnkLinkResourceConnection::Reconnect()
{
	return this->Connect();
}

/* Disconnect
 */
bool16 HttpLnkLinkResourceConnection::Disconnect()
{
    // Clear token and login credentials
    fLoginInfo.fToken = "";
    fLoginInfo.fUsername = "";
    fLoginInfo.fPassword = "";

	return kTrue;
}

/* GetConnectionId
 */
IHTTPLinkResourceConnection::ConnectionId HttpLnkLinkResourceConnection::GetConnectionId()
{
	return ConnectionId();
}

/* GetLoginCredentials
 */
bool16 HttpLnkLinkResourceConnection::GetLoginCredentials(std::stringstream& loginInformation) const
{
    JSON json;
    convertLoginInfoToJson(fLoginInfo, json);
    json.write_json(loginInformation);
    return kTrue;
}

/* SetLoginCredentials
 */
bool16 HttpLnkLinkResourceConnection::SetLoginCredentials(const std::stringstream& loginInformation)
{
    JSON json;
    std::stringstream loginstr(loginInformation.str());
    json.read_json(loginstr);
    return convertJsonToLoginInfo(fLoginInfo, json);
}


bool16 HttpLnkLinkResourceConnection::GenerateToken(std::string &token)
{
    // Generate token from API wrapper
    InterfacePtr<IHTTPLinkManager> httpLinkManager(GetExecutionContextSession(), UseDefaultIID());
    URI uri;
    WideString scheme(kHttpLnkScheme);
    uri.SetComponent(URI::kScheme, scheme);
    IHTTPLinkResourceServerAPIWrapper* serverAPIWrapper = httpLinkManager->GetHTTPLinkResourceServerAPIWrapper(uri);
    if (serverAPIWrapper->GenerateSessionToken(this, fLoginInfo.fToken))
    {
        return kTrue;
    }
    return kFalse;
}

void HttpLnkLinkResourceConnection::convertLoginInfoToJson(LoginInfo loginInfo, JSON& json)
{
    json.addValue(username_key, loginInfo.fUsername);
    json.addValue(password_key, loginInfo.fPassword);
    json.addValue(token_key, loginInfo.fToken);
}

bool16 HttpLnkLinkResourceConnection::convertJsonToLoginInfo(LoginInfo &loginInfo, JSON& logindetails)
{
    try {
        
        std::string username    = logindetails.GetString(username_key);
        std::string password    = logindetails.GetString(password_key);
        std::string token = "";
        //This is an optional parameter
        if (logindetails.checkKey(token_key))
            logindetails.GetString(token_key);
        
        loginInfo.fUsername     = username;
        loginInfo.fPassword     = password;
        loginInfo.fToken        = token;
        
        return kTrue;
        
    } catch (...) {
        ASSERT_FAIL("Invalid login credentials");
    }
    return kFalse;
}
