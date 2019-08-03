#!/usr/bin/python2

import os;

def read_file(path):
    f = open(path, "r");
    text = f.read();
    f.close();
    return text;

output = open("index.html", "w");
output.write(read_file("head.html"));


list = os.listdir("pages");
list.sort();

for file in list:
    text = read_file("pages/" + file);
    #text = str.replace(text, "\n\n", "\n");
    text = str.replace(text, "\n[code]\n", "[code]");
    text = str.replace(text, "\n[/code]\n", "[/code]");
    text = str.replace(text, "\n", "<br>\n");
    text = str.replace(text, "[file]", "<span class=\"file\">");
    text = str.replace(text, "[/file]", "</span>");
    text = str.replace(text, "[code]", "\n<div class=\"code\">\n");
    text = str.replace(text, "[/code]", "\n</div>\n");

    output.write("<div>\n");
    output.write("<h2>" + file[3:-5] + "</h2>\n");
    output.write(text);
    output.write("</div>\n");

output.write(read_file("tail.html"));

output.close();
