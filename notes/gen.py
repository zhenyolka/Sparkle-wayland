#!/usr/bin/python2
# -*- coding: utf-8 -*-

import os;

def read_file(path):
    f = open(path, "r");
    text = f.read();
    f.close();
    return text;

def make_page(name):
    text = read_file("pages/" + name + ".in");
    text = str.replace(text, "\n[code]\n", "[code]");
    text = str.replace(text, "\n[/code]\n", "[/code]");
    text = str.replace(text, "\n", "<br>\n");
    text = str.replace(text, "[file]", "<span class=\"file\">");
    text = str.replace(text, "[/file]", "</span>");
    text = str.replace(text, "[code]", "\n<div class=\"code\">\n");
    text = str.replace(text, "[/code]", "\n</div>\n");
    text = str.replace(text, "[title]", "<h2>");
    text = str.replace(text, "[/title]", "</h2>");

    output = open("output/" + name + ".html", "w");
    output.write(read_file("head.html"));
    output.write(text);
    output.write(read_file("tail.html"));


def make_pages():
    list = os.listdir("pages");
    for file in list:
        make_page(file[0:-3]);

def write_link(output, name, path):
    output.write("<a href=\"" + path + "\">");
    output.write(name);
    output.write("</a><br>\n");

def make_index():
    output = open("output/index.html", "w");
    output.write(read_file("head.html"));
    write_link(output, "Требования", "requirements.html");
    write_link(output, "Основы", "introduction.html");
    write_link(output, "Лог", "log.html");
    write_link(output, "Подробности", "details.html");
    write_link(output, "Автозапуск клиентов", "autolaunch.html");
    write_link(output, "Звук", "audio.html");
    write_link(output, "Настройки", "settings.html");
    write_link(output, "Мутные экраны", "screens.html");
    write_link(output, "Контакты", "contacts.html");
    output.write(read_file("tail.html"));

make_pages();
make_index();
