dojo.provide('opensrf.tests.testosrfContinueStatus');

dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
    var h = { status: "status", statusCode: 1 };
    this.osrfcontinuestatus = new osrfContinueStatus(h);
});

doh.register("osrfContinueStatusTests", [
//Begin Tests
{
  name: "osrfContinueStatusCreateTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfcontinuestatus);
    doh.assertTrue(this.osrfcontinuestatus.hash);
    doh.assertTrue(this.osrfcontinuestatus.hash.status === "status");
    doh.assertTrue(this.osrfcontinuestatus.hash.statusCode === 1);
    doh.assertTrue(this.osrfcontinuestatus._encodehash);
  }
},

{
  name: "osrfContinueStatus_statusTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfcontinuestatus.status("newStatus") === "newStatus");
    doh.assertTrue(this.osrfcontinuestatus.hash.status === "newStatus");
    doh.assertTrue(this.osrfcontinuestatus.status("multi", "args") === "newStatus");
  }
},

{
  name: "osrfContinueStatus_statusCodeTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfcontinuestatus.statusCode(2) === 2);
    doh.assertTrue(this.osrfcontinuestatus.hash.statusCode === 2);
    doh.assertTrue(this.osrfcontinuestatus.statusCode(3, "args") === 2);
  }
}

//End Tests
]);
