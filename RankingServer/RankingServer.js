var http = require('http');
var fs = require('fs');
var url = require('url');
var qs = require('querystring');
var mysql = require('mysql');

var connection = mysql.createConnection
({
    host    : 'localhost',
    port    : 3306,
    user    : 'root',
    password: '46033878',
    database: 'blue_marble_rank'
});

var app = http.createServer(function(request,response){
    var _url = request.url;
    var pathname = url.parse(_url,true).pathname;

    var result = "";
    if(_url == '/ranking')
    {
        connection.connect();
        connection.query('SELECT * FROM ranking', function(err, rows, fields)
        {
            if(err)
            {
                console.log(err);
                connection.end();
            }
            else
            {
                for (var i = 0; i < rows.length; i++) 
                {
                    console.log(rows[i].name + " : " + rows[i].score);

                    if(i == rows.length-1)
                    {
                        result+=rows[i].name + " : " + rows[i].score;
                    }
                    else
                    {
                        result+=rows[i].name + " : " + rows[i].score + "\n";
                    }
                }
                response.writeHead(200);
                response.end(result);
                connection.end();
            }
        });
    }
    else if(pathname == '/addRank' && request.method == 'POST')
    {
      result = "addRank";
      var body = '';
      request.on('data',function(data){
        body = body+data;
      })

      request.on('end',function()
      {
        var post = qs.parse(body);
        title = post.title;

        // console.log(post['name']);  //post[key] 로 해당 key의 value를 가져올 수 있음.
        // console.log(post['score']);
      })

      response.writeHead(200);
      response.end(result);
    }
    else
    {
        response.end("");
    }
});
app.listen(3000);
