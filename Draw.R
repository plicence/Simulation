modelname = "STATION1.data"
data = read.table(modelname)
attach(data)


x =V1
f1=V2

plot (f1, x, type ="l", xlab="x",ylab ="Fx(x)", col ="red", main = "Relief du graphe")
legend("topleft", legend = c("Théorique","Expérimental"), col =c("red"), lty =1:1, cex =0.8) 
