dojo.provide('opensrf.tests.testosrfMethodException');

dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
    var h = { status: "status", statusCode: 1 };
    this.osrfmethodex = new osrfMethodException(h);
});

doh.register("osrfMethodExceptionTests", [
//Begin Tests
{
  name: "osrfMethodExceptionCreateTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmethodex);
    doh.assertTrue(this.osrfmethodex.hash);
    doh.assertTrue(this.osrfmethodex.hash.status === "status");
    doh.assertTrue(this.osrfmethodex.hash.statusCode === 1);
    doh.assertTrue(this.osrfmethodex._encodehash);
  }
},

{
  name: "osrfMethodException_statusTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmethodex.status("newStatus") === "newStatus");
    doh.assertTrue(this.osrfmethodex.hash.status === "newStatus");
    doh.assertTrue(this.osrfmethodex.status("multi", "args") === "newStatus");
  }
},

{
  name: "osrfMethodException_statusCodeTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmethodex.statusCode(2) === 2);
    doh.assertTrue(this.osrfmethodex.hash.statusCode === 2);
    doh.assertTrue(this.osrfmethodex.statusCode(3, "args") === 2);
  }
}

//End Tests
]);
