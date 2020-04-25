echo Compiling pdf...
pdflatex -synctex=1 -interaction=nonstopmode -shell-escape main.tex

echo Making Glossaries
makeindex main.glo -s main.ist -o main.gls

echo Compiling pdf...
pdflatex -synctex=1 -interaction=nonstopmode -shell-escape main.tex
