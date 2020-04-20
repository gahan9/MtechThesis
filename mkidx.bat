REMdel *.acn
REM del *.aux
REM del *.glo
REM del *.ilg
REM del *.ist
REM del *.lof
REM del *.log
REM del *.out
REM del *.toc

REM del *.gls

echo Making Glossaries
makeindex main.glo -s main.ist -o main.gls

REM echo Compiling pdf...
REM pdflatex.exe -synctex=1 -interaction=nonstopmode -shell-escape main.tex
