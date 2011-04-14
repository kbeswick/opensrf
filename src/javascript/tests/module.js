dojo.provide("opensrf.tests.module");
dojo.require("DojoSRF");

try{
	dojo.require("opensrf.tests.testJSON_v1");
	dojo.require("opensrf.tests.testosrfMessage");
	dojo.require("opensrf.tests.testosrfMethod");
}catch(e){
	doh.debug(e);
}
