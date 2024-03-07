#include <Arduino.h>
#include <rpccommand.h>
#include <logging.h>
#include <allcommands.h>
#include <app_config.h>
#include <net_manager.h>
#include  "sha256.h"

char AuthNeeded::lastNonce[40] ;
void strToHex(const char* src, char* dest, int len);

void AuthNeeded::executeCommand()
{
    logLineLevel(10, "Authneeded command being run");
    return401();
}

/**
 * return a result which is blank
 */
void AuthNeeded::return401()
{
    sprintf(lastNonce, "%lu%lu", millis(), random(1000000));
    const char *responseFmt = "{'src':'%s','error':{'code': '401', 'message':'{\\'realm\\':\\'%s\\', \\'nonce\\': \\'%s\\', \\'nc\\':1}'}}";
    char response[500];
    sprintf(response, responseFmt, JUICER_MACID, JUICER_MACID, lastNonce);
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}


/**
 * return a result which is blank
 */
void AuthNeeded::return403()
{
    const char *responseFmt = "{'src':'%s','error':{'code': '403'}}";
    char response[500];
    sprintf(response, responseFmt, JUICER_MACID);
    replaceQuotes(response);
    mpCommandSource->sendResponse(response);
}

bool AuthNeeded::needsAuthentication(){
    if (!GlobalState::getInstance()->hasProperty(PROP_AUTH_PASSWORD)) return false ;
    String sProp = GlobalState::getInstance()->getPropertyStr(PROP_AUTH_PASSWORD);
    return sProp.length() > 0 ;
}

bool AuthNeeded::validateRequest(){
    // here validate the hash for the request.
    if (mpCommandJSON->containsKey("auth") &&
    ((*mpCommandJSON)["auth"]).containsKey("response") &&
    ((*mpCommandJSON)["auth"]).containsKey("cnonce") &&
    ((*mpCommandJSON)["auth"]).containsKey("nonce")
    ){
        String resp = (*mpCommandJSON)["auth"]["response"];
        String nonce = (*mpCommandJSON)["auth"]["nonce"];
        String cnonce = (*mpCommandJSON)["auth"]["cnonce"];
        String sProp = GlobalState::getInstance()->getPropertyStr(PROP_AUTH_PASSWORD);
        SHA256 *pSHA = new SHA256();
        char rat[50] = {0};
        char rathash[35] = {0};
        char rathashstr[65] = {0};
        
        sprintf(rat, "%s:%s", "dummy_method", "dummy_uri");
        pSHA->update(rat, strlen(rat));
        pSHA->finalize(rathash, 35);
        pSHA->clear();
        strToHex(rathash, rathashstr, pSHA->HASH_SIZE);

        char lat[500] = {0};
        char lathash[35] = {0};
        char lathashstr[65] = {0};
        sprintf(lat, "%s:%s:%d:%s:auth:%s", sProp.c_str(), nonce, 1, cnonce, rathashstr);
        pSHA->update(lat, strlen(lat));
        pSHA->finalize(lathash, 35);
        pSHA->clear();
        strToHex(lathash, lathashstr, pSHA->HASH_SIZE);
        delete pSHA;
        if (!strcmp(lathashstr, resp.c_str())) return true ; // has matches response !
    }

    return403();
    return false ;
}

/**
 * convert binary to hex, be careful to call with destination having twice the size of the source
*/
void strToHex(const char* src, char* dest, int len){
    char* pDest = dest ;
    for (int i = 0; i < len; i++){
        sprintf(pDest, "%02x", src[i]);
        pDest += 2 ;
    }    
}