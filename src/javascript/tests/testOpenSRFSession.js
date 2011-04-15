dojo.provide('opensrf.tests.testOpenSRFSession');

dojo.require('sinon-1.0.0', true);
dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
  this.osrfSes = new OpenSRF.Session();
  OpenSRF.Session.cache["thread"] = "trace";
});

doh.register("OpenSRFSessionTests", [

  {
    name: "OpenSRFSessionCreateTest",
    setUp: setupFunc,
    runTest: function () {
      doh.assertTrue(this.osrfSes);
      doh.assertTrue(this.osrfSes.remote_id == null);
      doh.assertTrue(this.osrfSes.state === OSRF_APP_SESSION_DISCONNECTED);
    }
  },

  {
    name: "OpenSRFSession_FindSessionTest",
    setUp: setupFunc,
    runTest: function () {
      doh.assertTrue(OpenSRF.Session.find_session("thread") === "trace");
    }
  },

  {
    name: "OpenSRFSession_send_xhrTest",
    setUp: setupFunc,
    runTest: function () {
      doh.assertTrue(OpenSRF.Session.find_session("thread") === "trace");
    }
  }

]);
