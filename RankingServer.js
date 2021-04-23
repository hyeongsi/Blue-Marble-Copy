var http = require('http');
var fs = require('fs');
var url = require('url');
var qs = require('querystring');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var pathname = url.parse(_url,true).pathname;

    var result;
    if(_url == '/ranking'){
      //result = fs.readFileSync('ranking.json', 'utf8');
      // db 연결해가지고 데이터 보내는거 구현하기.
      result = "ranking Data Send";
    }
    else if(pathname == '/addRank' && request.method == 'POST')
    {
        result = "addRank";
      var body = '';
      request.on('data',function(data){
        body = body+data;
      })

      request.on('end',function(){
        var post = qs.parse(body);
        title = post.title;

        console.log(post['name']);  //post[key] 로 해당 key의 value를 가져올 수 있음.
        console.log(post['score']);
      })

    }

    //var data = JSON.parse(title); 이것처럼 문자열을 json으로 파싱해서 저장 하면 될듯

    console.log(result);
    response.writeHead(200);
    response.end(result);
});
app.listen(3000);