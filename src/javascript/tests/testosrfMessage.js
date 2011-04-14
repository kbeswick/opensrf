dojo.provide('opensrf.tests.testosrfMessage');

dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
  var h = { locale: "" };
  this.osrfmsg = new osrfMessage(h);
});

doh.register("osrfMessageTests", [
//Begin Tests
{
  name: "osrfMessageCreateTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmsg.hash.locale === 'en-US');
    doh.assertTrue(this.osrfmsg._encodehash);
  }
},

{
  name: "osrfMessage_threadTraceTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmsg.threadTrace() == undefined);
    doh.assertTrue(this.osrfmsg.threadTrace("abc") === "abc");
    doh.assertTrue(this.osrfmsg.threadTrace("123", "456") === "abc");
    doh.assertTrue(this.osrfmsg.hash.threadTrace === "abc");
  }
},

{
  name: "osrfMessage_typeTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmsg.type() == undefined);
    doh.assertTrue(this.osrfmsg.type("type") === "type");
    doh.assertTrue(this.osrfmsg.type("multi", "argument") === "type");
    doh.assertTrue(this.osrfmsg.hash.type === "type");
  }
},

{
  name: "osrfMessage_payloadTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmsg.payload() == undefined);
    doh.assertTrue(this.osrfmsg.payload("pay") === "pay");
    doh.assertTrue(this.osrfmsg.payload("multi", "argument") === "pay");
    doh.assertTrue(this.osrfmsg.hash.payload === "pay");
  }
},

{
  name: "osrfMessage_localeTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmsg.locale() == "en-US");
    doh.assertTrue(this.osrfmsg.locale("fr-CA") === "fr-CA");
    doh.assertTrue(this.osrfmsg.locale("multi", "argument") === "fr-CA");
    doh.assertTrue(this.osrfmsg.hash.locale === "fr-CA");
  }
},

{
  name: "osrfMessage_typeTest",
  setUp: function() {
    var h = { locale: "" };
    this.osrfmsg = new osrfMessage(h);

    this.res = {
      "__c": "osrfMessage",
      "__p": {
        "threadTrace" : null,
        "type" : null,
        "payload" : "null",
        "locale" : "en-US"
      }
    };
  },
  runTest: function() {
    //XXX there is probably a better way of comparing two js objects
    doh.assertTrue(js2JSON(this.osrfmsg.serialize()) === js2JSON(this.res));
    //TODO test when there is a payload
  }
}

//End Tests
]);
