dojo.provide('opensrf.tests.testosrfServerError');

dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
    var h = { status: "status", statusCode: 1 };
    this.osrfservererror = new osrfServerError(h);
});

doh.register("osrfServerErrorTests", [
//Begin Tests
{
  name: "osrfServerErrorCreateTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfservererror);
    doh.assertTrue(this.osrfservererror.hash);
    doh.assertTrue(this.osrfservererror.hash.status === "status");
    doh.assertTrue(this.osrfservererror.hash.statusCode === 1);
    doh.assertTrue(this.osrfservererror._encodehash);
  }
},

{
  name: "osrfServerError_statusTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfservererror.status("newStatus") === "newStatus");
    doh.assertTrue(this.osrfservererror.hash.status === "newStatus");
    doh.assertTrue(this.osrfservererror.status("multi", "args") === "newStatus");
  }
},

{
  name: "osrfServerError_statusCodeTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfservererror.statusCode(2) === 2);
    doh.assertTrue(this.osrfservererror.hash.statusCode === 2);
    doh.assertTrue(this.osrfservererror.statusCode(3, "args") === 2);
  }
}

//End Tests
]);
