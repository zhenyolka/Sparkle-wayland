#!/usr/bin/python2
# -*- coding: utf-8 -*-

import os;
import shutil;

def read_file(path):
    f = open(path, "r");
    text = f.read();
    f.close();
    return text;

def make_page(language, page):
    print(language, page);
    name = page[0:-3];
    text = read_file("input/" + language + "/" + page);
    text = text.replace("\n", "<br>\n");
    text = text.replace("\\<br>", "");
    text = text.replace("    ", "&nbsp;&nbsp;&nbsp;&nbsp;");
    text = text.replace("*", "&bull;");
    text = text.replace("<file>", "<span class=\"file\">");
    text = text.replace("</file>", "</span>");
    text = text.replace("<code>", "<div class=\"code\">");
    text = text.replace("</code>", "</div>");

    output = open("output/" + language + "/" + name + ".html", "w");
    output.write(read_file("head.html"));
    output.write("<div class=\"text\">\n");
    output.write(text);
    output.write("</div>\n");
    output.write(read_file("tail.html"));
    output.close();


def make_pages(language):
    list = os.listdir("input/" + language);
    for page in list:
        make_page(language, page);

def make():
    list = os.listdir("input");
    for language in list:
        os.mkdir("output/" + language);
        shutil.copyfile("style.css", "output/" + language + "/style.css");
        make_pages(language);

shutil.rmtree("output");
os.mkdir("output");
make();
