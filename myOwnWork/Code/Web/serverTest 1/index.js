var mongodb = require('mongodb');
var MongoClient = mongodb.MongoClient;
var url = 'mongodb://localhost:27017/thesisTest_1'

MongoClient.connect(url, function(err, db){
    if (err){
        console.log('Unable to connect. Error: ', err);
    }else{
        console.log('Connected to', url);
    db.close();
    }
});