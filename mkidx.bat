del *.acn
del *.aux
del *.glo
del *.ilg
del *.ist
del *.lof
del *.log
del *.out
del *.toc

del main.gls && makeindex main.glo -s main.ist -o main.gls
