console.clear();

const express = require("express");
const app = express();
const server = require("http").Server(app);
const io = require("socket.io")(server);
const path = require("path");

app.set("view engine", "ejs");
app.set("views", path.join(__dirname, "views"));

app.use(express.static(path.join(__dirname, "./Public")));

app.get("/", (req, res) => {
	res.render("index");
});

let inventario = 0;
let CajasRojas = 0;
let CajasVerdes = 0;
let CajasAzules = 0;
let EstadoBrazo = "Inactivo";

io.on("connection", (socket) => {
	console.log("Nuevo Cliente Conectado");
	socket.on("Estado", (valor) => {
		EstadoBrazo = valor.Estado;
		console.log(EstadoBrazo);
		socket.broadcast.emit("Estado", valor.Estado);
	});

	socket.on("info", (dato) => {
		socket.emit("datos", {
			inventario: inventario,
			CajasRojas: CajasRojas,
			CajasVerdes: CajasVerdes,
			CajasAzules: CajasAzules,
			Estado: EstadoBrazo,
		});
	});

    socket.on('Inventario',dato=>{
        socket.broadcast.emit('Inventario',dato);
    });

    socket.on('Cajas',dato=>{
        if(dato.Rojo != undefined){
			CajasRojas++;
			socket.broadcast.emit('CajaRoja',CajasRojas);
		}
		if(dato.Verde != undefined){
			CajasVerdes++;
			socket.broadcast.emit('CajaVerde',CajasVerdes);
		}
		if(dato.Azul != undefined){
			CajasAzules++;
			socket.broadcast.emit('CajaAzul',CajasAzules);
		}
        inventario-=1;
        socket.broadcast.emit('inventario',inventario);
        console.log(dato);
    });
});

server.listen(5000, "192.168.43.91", () => {
	console.log("El servidor se inicio en el puerto 5000");
});
