#!/bin/sh
set -xe

cd Relatorio-RC-Meta_2/ || exit 1

pdflatex main.tex

cp main.pdf ../relatorio.pdf
