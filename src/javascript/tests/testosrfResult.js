dojo.provide('opensrf.tests.testosrfResult');

dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
    var h = { status: "status", statusCode: 1, content: "content" };
    this.osrfresult = new osrfResult(h);
});

doh.register("osrfResultTests", [
//Begin Tests
{
  name: "osrfResultCreateTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfresult);
    doh.assertTrue(this.osrfresult.hash);
    doh.assertTrue(this.osrfresult.hash.status === "status");
    doh.assertTrue(this.osrfresult.hash.statusCode === 1);
    doh.assertTrue(this.osrfresult.hash.content === "content");
    doh.assertTrue(this.osrfresult._encodehash);
  }
},

{
  name: "osrfResult_statusTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfresult.status("newStatus") === "newStatus");
    doh.assertTrue(this.osrfresult.hash.status === "newStatus");
    doh.assertTrue(this.osrfresult.status("multi", "args") === "newStatus");
  }
},

{
  name: "osrfResult_statusCodeTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfresult.statusCode(2) === 2);
    doh.assertTrue(this.osrfresult.hash.statusCode === 2);
    doh.assertTrue(this.osrfresult.statusCode(3, "args") === 2);
  }
},

{
  name: "osrfResult_contentTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfresult.content("stuff") === "stuff");
    doh.assertTrue(this.osrfresult.hash.content === "stuff");
    doh.assertTrue(this.osrfresult.content("multi", "args") === "stuff");
  }
}

//End Tests
]);
