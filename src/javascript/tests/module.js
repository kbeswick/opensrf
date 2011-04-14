dojo.provide("opensrf.tests.module");
dojo.require("DojoSRF");

try{
	dojo.require("opensrf.tests.testJSON_v1");
	dojo.require("opensrf.tests.testosrfMessage");
	dojo.require("opensrf.tests.testosrfMethod");
	dojo.require("opensrf.tests.testosrfMethodException");
	dojo.require("opensrf.tests.testosrfConnectStatus");
	dojo.require("opensrf.tests.testosrfResult");
	dojo.require("opensrf.tests.testosrfServerError");
	dojo.require("opensrf.tests.testosrfContinueStatus");

}catch(e){
	doh.debug(e);
}
