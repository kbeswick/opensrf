dojo.provide('opensrf.tests.testosrfMethod');

dojo.require('opensrf.opensrf', true);

var setupFunc = (function() {
    var h = { method: "someMethod" };
    this.osrfmethod = new osrfMethod(h);
});

doh.register("osrfMethodTests", [
//Begin Tests
{
  name: "osrfMethodCreateTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmethod);
    doh.assertTrue(this.osrfmethod.hash);
    doh.assertTrue(this.osrfmethod._encodehash);
    doh.assertTrue(this.osrfmethod.hash.method === "someMethod");
  }
},

{
  name: "osrfMethod_methodTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmethod.method("otherMethod") === "otherMethod");
    doh.assertTrue(this.osrfmethod.hash.method === "otherMethod");
    doh.assertTrue(this.osrfmethod.method("multi", "args") === "otherMethod");
  }
},

{
  name: "osrfMethod_paramsTest",
  setUp: setupFunc,
  runTest: function() {
    doh.assertTrue(this.osrfmethod.params("param1") === "param1");
    doh.assertTrue(this.osrfmethod.hash.params === "param1");
    doh.assertTrue(this.osrfmethod.params("multi", "args") === "param1");
  }
},

{
  name: "osrfMethod_serializeTest",
  setUp: function() {
    var h = { method: "someMethod", params: "param1" };
    this.osrfmethod = new osrfMethod(h);
    this.res = {
      "__c" : "osrfMethod",
      "__p" : {
        "method" : "someMethod",
        "params" : "param1"
      }
    };
  },
  runTest: function() {
    doh.assertTrue(js2JSON(this.osrfmethod.serialize()) === js2JSON(this.res));
  }
}



//End Tests
]);
