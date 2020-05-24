modelname = "STATION10.data"
data = read.table(modelname)
attach(data)

x =V1
f1=V2

cat(mean(x))
cat(sqrt(mean(x^2)))
cat(var(x))
cat(sd(x))
plot (f1, x, type ="l", xlab="x",ylab ="Fx(x)", col ="red", main = "Relief du graphe")
legend("topleft", legend = c("Théorique","Expérimental"), col =c("red"), lty =1:1, cex =0.8) 
