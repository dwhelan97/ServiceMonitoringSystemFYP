var express = require('express');
var loggedIn = false;
var router = express.Router();
var assert = require('assert');
var Handlebars = require('handlebars');
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";
// /////////////////////////////AWS
var IncomingMessage = {
  contents: ""
};
var awsIot = require('aws-iot-device-sdk');
var device = awsIot.device({
  keyPath: "certs/private.pem.key",
  certPath: "certs/certificate.pem.crt",
  caPath: "certs/aws-root-ca.pem",
  clientId: "myTestThing1",
  host: "a151wdf1sc5r8s-ats.iot.us-east-1.amazonaws.com"
});

device
  .on('connect', function() {
    console.log('connect');
    device.subscribe('myTopic/1');
    device.publish('myTopic/2', JSON.stringify({
      message: 'Connection Test'
    }));
  });

var obj;
var arr = [];
let iii = 0;
device
  .on('message', function(topic, payload) {
    iii++;
    var message = payload.toString();
    obj = JSON.parse(message);
    arr.push(obj);
    IncomingMessage.contents = payload.toString();
  });
router.get('/Mqtt3', function(req, res, next) {
  device.publish("myTopic/2", JSON.stringify({
    message: 'AJ1'
  }));
  arr.shift();
   res.render('ActiveJobsPage', {
     arr: arr
   })
  arr = [];
   iii = 0;
});

router.get('/Mqtt', function(req, res, next) {
  device.publish("myTopic/2", JSON.stringify({
    message: 'AJ2'
  }));
  res.json(arr[0]);
  arr = [];
});
router.get('/AJ', function(req, res, next) {
  res.render('ActiveJobsPage');
});
router.get('/CJ', function(req, res, next) {
  res.render('CurrentJobs');
});
router.get('/', function(req, res, next) {
  res.render('login');
});
router.get('/P', function(req, res, next) {
  res.render('PricingPage');
});
router.get('/C', function(req, res, next) {
  res.render('ContactPage');
});
router.get('/CP', function(req, res, next) {
  res.render('customerPage');
});
router.get('/MD', function(req, res, next) {
  res.render('MachineDiag');
});
router.get('/MS', function(req, res, next) {
  res.render('MachineService');
});
router.post('/insert', function(req, res, next) {
  var item = {
    User: req.body.username,
    Password: req.body.psw
  };
  console.log(item);
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("userDb");
    dbo.collection("Users").insertOne(item, function(err, result) {
      assert.equal(null, err);
      console.log('Item Inserted Successfully');
    });
    dbo.close();
  });
  AdminLoggedIN = true;
  res.redirect('/login');
});



router.post('/delete', function(req, res, next) {
  var item = {
    User: req.body.User1,
  };
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("userDb");
    dbo.collection("Users").deleteOne(item, function(err, result) {
      assert.equal(null, err);
      console.log('Deletion Successfull');
    });
    dbo.close();
  });
  AdminLoggedIN = true;
  res.redirect('/login');
});



router.get('/login', function(req, res, next) {
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("userDb");
    var arr = dbo.collection("Users").find().toArray(function(err, result) {
      console.log("Admin Logged In: " + AdminLoggedIN);
      if (err) throw err;
      if (AdminLoggedIN || loggedIn) {
        if (AdminLoggedIN) {
          res.render('adminPage', {
            title: "Admin Page",
            items: result
          })
        } else {
          res.render('index', {
            title: "My Page"
          });
        }
      }
      db.close();
    });
  });
});



router.post('/insert', function(req, res, next) {
  var item = {
    User: req.body.username,
    Password: req.body.psw
  };
  console.log(item);
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("userDb");
    dbo.collection("Users").insertOne(item, function(err, result) {
      assert.equal(null, err);
      console.log('Insertion Successfull');
    });
    dbo.close();
  });
  res.redirect('/');
});

router.post('/login', function(req, res, next) {
  var user = {
    name: req.body.uname,
    password: req.body.psw
  };
  var attemptedUser = {
    name: "",
    password: "",
    setDetails(name, password) {
      this.name = name;
      this.password = password;
    }
  };
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("userDb");
    var arr = dbo.collection("Users").find({
      User: req.body.uname
    }).toArray(function(err, result) {
      if (err) throw err;
      attemptedUser.setDetails(result[0].User, result[0].Password);
    });
  });
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("userDb");
    var arr = dbo.collection("Users").find().toArray(function(err, result) {
      if (err) throw err;
      if (user.name == attemptedUser.name && user.password == attemptedUser.password) {
        loggedIn = true;
        if (user.name == "Admin") {
          res.render('adminPage', {
            items: result
          })
        } else {
          res.render('customerPage', {
            title: "My Page",
            username: user.name
          });
        }
      } else {
        res.render('error', {
          message: "Details incorrect",
          username: user.name,
          password: user.password
        });
      }
      db.close();
    });
  });
  router.get('/AP', function(req, res, next) {
    MongoClient.connect(url, function(err, db) {
      if (err) throw err;
      var dbo = db.db("userDb");
      var arr = dbo.collection("Users").find().toArray(function(err, result) {
        if (err) throw err;
        res.render('adminPage', {
          items: result
        });
      });
    });
  });
});
module.exports = router;
