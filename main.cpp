#include <iostream>
#include <vector>
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <windows.h>
#include <json.hpp>

#ifdef __cplusplus
extern "C"
{
    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
}
#endif

using namespace std;
using njson = nlohmann::json;

// <center>
//    <img src="123.jpg" width="800px">
//    <video controls width="800"><source src="video3.mp4" type="video/mp4"></video>
// </center>
	
string index_htmlstart = R"(
<! DOCTYPE HTML>
<html>
<head>
<title>video browser</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
</head>    
<body>)";
string index_htmlend = R"(</body></html>)";
string htmltmp;

string basedir="./www";

int check_file_type(const char *filename)
{
    if(strstr(filename, ".avi") != NULL)
        return 1;
    else if(strstr(filename, ".mp4") != NULL)
        return 2;
    else if(strstr(filename, ".ogg") != NULL)
        return 3;
    else 
        return 0;
}

void from_txt_getname(string listtxt, vector<tuple<string,string>> &tname)
{
    FILE *fd=NULL;
    fd = fopen(listtxt.c_str(), "r");
    if(fd == NULL) {
      perror("打开文件时发生错误");
      return;
    }
    char fbuff[512]={0};
    while (fgets(fbuff, 512, fd) != NULL)
    {
        if(fbuff[0]!=' '&&fbuff[0]!='\n')
        {
            //cout<<fbuff;
            strtok(fbuff," ");
            strtok(NULL," ");
            string type(strtok(NULL," "));
            if(type=="<DIR>")
            {
                string name(strtok(NULL," "));
                name.pop_back();
                tname.push_back(make_tuple(type, name));
            }
            else
            {
                string name(strtok(NULL," "));
                name.pop_back();
                tname.push_back(make_tuple("<FILE>",name));
            }
        }
    }
    fclose(fd);
}

//生成html中body里的内容
string getdir_detail(string dirpath, int listnum, int width)
{
    string html="";
    string cmd("dir "+dirpath+" > ./list.txt");
    replace(cmd.begin(), cmd.end(), '/', '\\');
    //cout<<cmd.c_str()<<endl;
    system(cmd.c_str());

    vector<tuple<string,string>> files;
    from_txt_getname("./list.txt", files);
    
    int numtmp=0;
    for(size_t i=0;i<files.size();i++)
    {
        string type = get<0>(files[i]);
        string name = get<1>(files[i]);
        //cout<<get<0>(files[i])<<"--"<<get<1>(files[i])<<endl;
        if(numtmp == 0)
        {
            html.append("<center>\r\n");
        }
        if(type == "<DIR>")
        {
            if(name == "..")
            {
                string tmp = dirpath;
                if(tmp!="./www/video/")
                {
                    tmp.pop_back();
                    while(tmp.substr(tmp.length()-1)!="/")
                    {
                        tmp.pop_back();
                    }
                    html.append("<a href=\""+tmp.substr(strlen("./www"))+"\"><img src=\"/dir.jpg\"");
                    html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
                }
                else
                {
                    html.append("<a><img src=\"/dir.jpg\"");
                    html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
                }
            }
            else if(name == ".")
            {
                html.append("<a><img src=\"/ff.jpg\"");
                html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
            }
            else
            {
                html.append("<a href=\""+dirpath.substr(strlen("./www"))+name+"/"+"\"><img src=\"/dir.jpg\"");
                html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
            }
        }
        else if(type == "<FILE>")
        {
            int ret=0;
            if(ret = check_file_type(name.c_str())) {
                html.append("<video controls width=\""+to_string(width)+"\"><source src=\""+dirpath.substr(strlen("./www"))+name+"\" type=\"");
                if(ret == 1 || ret == 2)
                    html.append("video/mp4\"></video>\r\n");
                else if(ret == 3)
                    html.append("video/ogg\"></video>\r\n");
            }
            else {
                html.append("<span>"+name+"  文件格式不支持"+"</span>\r\n");
            }
        }
        numtmp++;
        if(numtmp==listnum) {
            html.append("<center>\r\n");
            numtmp = 0;
        }
    }
    if(numtmp!=0)
        html.append("<center>\r\n");
    //cout<<html<<endl;
    return html;
}

void video_callback(const httplib::Request& req, httplib::Response& res) {
    string targetdir(basedir+req.path);
    //在最后添加符号  /
    if(targetdir.substr(targetdir.length()-1)!="/")
        targetdir.append("/");
    string tmp = getdir_detail(targetdir, 3, 300);
    htmltmp = "";
    htmltmp.append(index_htmlstart+tmp+index_htmlend);
    res.set_content(htmltmp, "text/html");
}

void error_callback(const httplib::Request& req, httplib::Response& res) {
    const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), fmt, res.status);
    res.set_content(buf, "text/html");
}

int main(int argc, char *argv[])
{
    httplib::Server ser;
    if(!ser.is_valid())
    {
        cout<<"error"<<endl;
        return -1;
    }

    ser.set_base_dir(basedir);
    //允许video/的任意web路径进入回调函数处理
    ser.Get(R"(/video/(.*))", video_callback);
    ser.set_error_handler(error_callback);

    ser.listen("0.0.0.0", 8080);
    return 0;
}

