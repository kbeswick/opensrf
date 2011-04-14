dojo.provide('opensrf.tests.testosrfConnectStatus');

dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
    var h = { status: "status", statusCode: 1 };
    this.osrfconnstatus = new osrfConnectStatus(h);
});

doh.register("osrfConnectStatusTests", [
//Begin Tests
{
  name: "osrfConnectStatusCreateTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfconnstatus);
    doh.assertTrue(this.osrfconnstatus.hash);
    doh.assertTrue(this.osrfconnstatus.hash.status === "status");
    doh.assertTrue(this.osrfconnstatus.hash.statusCode === 1);
    doh.assertTrue(this.osrfconnstatus._encodehash);
  }
},

{
  name: "osrfConnectStatus_statusTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfconnstatus.status("newStatus") === "newStatus");
    doh.assertTrue(this.osrfconnstatus.hash.status === "newStatus");
    doh.assertTrue(this.osrfconnstatus.status("multi", "args") === "newStatus");
  }
},

{
  name: "osrfConnectStatus_statusCodeTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfconnstatus.statusCode(2) === 2);
    doh.assertTrue(this.osrfconnstatus.hash.statusCode === 2);
    doh.assertTrue(this.osrfconnstatus.statusCode(3, "args") === 2);
  }
}

//End Tests
]);
