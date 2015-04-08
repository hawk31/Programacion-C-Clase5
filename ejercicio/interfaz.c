#include <gtk/gtk.h>
#include <stdlib.h>
#include "concesionario.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORTNUM 9999


struct concesionario *con;
int id;

/* snprintf */

static int ide_snprintf(GtkButton *widget, gpointer data)
{
	char buffer[8000];
	int binding;
	int allowreuse = 1;
	int reuse;


	struct sockaddr_in dest;
	struct sockaddr_in serv;
	int server;
	socklen_t socksize = sizeof(struct sockaddr_in);

	curso_concesionario_snprintf(buffer, sizeof(buffer), con);
	printf("%s\n", buffer);

	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	serv.sin_port = htons(PORTNUM);

	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0) {
		printf("Error: Socket()\n");
		return -1;
	}

	reuse = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &allowreuse, sizeof allowreuse);
	if(reuse < 0){
		perror("Reuse \n");
		return -1;
	}


	binding = bind(server, (struct sockaddr *)&serv, sizeof(struct sockaddr));

	if(binding < 0){
		perror("Error binding \n");
		return -1;
	}

	listen(server, 1);

	int client = accept(server, (struct sockaddr *)&dest, &socksize);
	if (client < 0) {
		printf("Error: Accept()\n");
		return -1;
	}

	while(client) {
		printf("IP %s \n", inet_ntoa(dest.sin_addr));
		send(client, buffer, strlen(buffer), 0);
		close(client);
		client = 0;
	}

	close(server);


}


/* Borrar coche */

static int delete_car(GtkButton *widget, gpointer data){
	GtkWidget **entrybor;
	GtkWidget *entry_ptr_pos;

	entrybor = (GtkWidget **)data;

	int pos;

	entry_ptr_pos = entrybor[0];
	pos = atoi(gtk_entry_get_text((GtkEntry *)entry_ptr_pos));
	/*printf("%d\n", pos);*/


	uint32_t max;
	max = curso_concesionario_attr_get_u32(con, CURSO_CONCESIONARIO_ATTR_NUM_COCHES);

	if(pos>=max){
		printf("No existe el coche en la posicion %d \n", pos);
		return -1;
	}

	curso_concesionario_attr_unset_coche(con, pos);
	printf("Car successfully deleted \n");
}


/* Ventana borrar */

static void borrarCoche(GtkButton *widget, gpointer data){
	GtkWidget *windowborrar;
	GtkWidget *entrypos;
	GtkWidget *sendBorrar;
	GtkWidget **entrybor;
	GtkWidget *boxbor;

	entrybor = calloc(0,1*sizeof(GtkWidget));

	windowborrar = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	entrypos = gtk_entry_new();
	sendBorrar = gtk_button_new_with_label("Borrar coche");
	boxbor = gtk_vbox_new(TRUE, 5);

	entrybor[0] = entrypos;

	gtk_box_pack_start((GtkBox *)boxbor, entrypos, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)boxbor, sendBorrar, TRUE, TRUE, 2);
	gtk_container_add((GtkContainer *)windowborrar, boxbor);

	/*Señal borrar*/
	g_signal_connect((GObject *)sendBorrar, "clicked", (GCallback)delete_car,
		(gpointer)entrybor);


	gtk_widget_show_all(windowborrar);


}


/* Modificar coche */

static int modify_car(GtkButton *widget, gpointer data)
{
	GtkWidget **entrymod;
	GtkWidget *entry_ptr_marca;
	GtkWidget *entry_ptr_matricula;
	GtkWidget *entry_ptr_pos;
	struct coche *c;

	const gchar *marca, *matricula;
	int pos;

	entrymod = (GtkWidget **)data;
	entry_ptr_marca = entrymod[0];
	entry_ptr_matricula = entrymod[1];
	entry_ptr_pos = entrymod[2];

	matricula = gtk_entry_get_text((GtkEntry *)entry_ptr_matricula);
	marca = gtk_entry_get_text((GtkEntry *)entry_ptr_marca);
	pos = atoi(gtk_entry_get_text((GtkEntry *)entry_ptr_pos));

	/*printf("%s,%s,%d\n", matricula, marca, pos);*/

	uint32_t max;
	max = curso_concesionario_attr_get_u32(con, CURSO_CONCESIONARIO_ATTR_NUM_COCHES);

	if(pos>=max){
		printf("No existe el coche en la posicion %d \n", pos);
		return -1;
	}

	c = curso_coche_alloc();
	curso_coche_attr_set_str(c, CURSO_COCHE_ATTR_MATRICULA, matricula);
	curso_coche_attr_set_str(c, CURSO_COCHE_ATTR_MARCA, marca);
	curso_concesionario_attr_unset_coche(con, pos);	
	curso_concesionario_attr_set_coche(con, CURSO_CONCESIONARIO_ATTR_COCHE, c);
	printf("Success modifying car \n");


	return 1;
}

/* Ventana modificar */

static void modificarCoche(GtkButton *widget, gpointer data)
{
	GtkWidget *windowmodificar;
	GtkWidget *entrymarca, *entrymatricula, *entrypos;
	GtkWidget *sendModificar;
	GtkWidget *boxmodificar;
	GtkWidget **entrymod;

	entrymod = calloc(0, 3*sizeof(GtkWidget));

	windowmodificar = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	entrymarca = gtk_entry_new();
	entrymatricula = gtk_entry_new();
	entrypos = gtk_entry_new();
	sendModificar = gtk_button_new_with_label("Modificar Coche");


	boxmodificar = gtk_vbox_new(TRUE, 2);

	entrymod[0] = entrymarca;
	entrymod[1] = entrymatricula;
	entrymod[2] = entrypos;

	gtk_box_pack_start((GtkBox *)boxmodificar, entrymarca, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)boxmodificar, entrymatricula, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)boxmodificar, entrypos, TRUE, TRUE, 2);
	gtk_box_pack_start((GtkBox *)boxmodificar, sendModificar, TRUE, TRUE, 2);



	gtk_container_add((GtkContainer *)windowmodificar, boxmodificar);

	/*Señal modificar*/
	g_signal_connect((GObject *)sendModificar, "clicked", (GCallback)modify_car,
		(gpointer)entrymod);

	gtk_widget_show_all(windowmodificar);



}


/*Añadir coche*/

static void add_car(GtkButton *widget, gpointer data)
{
	GtkWidget **entry;
	struct coche *c;
	GtkWidget *entry_ptr_marca;
	GtkWidget *entry_ptr_matricula;
	GtkWidget *entry_ptr_id;

	const gchar *matricula, *marca;

	entry = (GtkWidget **)data;
	entry_ptr_marca = entry[0];
	entry_ptr_matricula = entry[1];
	entry_ptr_id = entry[2];

	matricula = gtk_entry_get_text((GtkEntry *)entry_ptr_matricula);
	marca = gtk_entry_get_text((GtkEntry *)entry_ptr_marca);
	/*id = gtk_entry_get_text((GtkEntry *)entry_ptr_id);*/

	/*printf("%s,%s,%d\n", matricula, marca, id);*/

	c = curso_coche_alloc();

	curso_coche_attr_set_u32(c, CURSO_COCHE_ATTR_ID, id++);
	curso_coche_attr_set_str(c, CURSO_COCHE_ATTR_MATRICULA, matricula);
	curso_coche_attr_set_str(c, CURSO_COCHE_ATTR_MARCA, marca);

	curso_concesionario_attr_set_coche(con, CURSO_CONCESIONARIO_ATTR_COCHE, c);
	printf("Success adding car \n");
}


/* Ventana anadir */

static void anadirCoche(GtkButton *widget, gpointer data)
{
	GtkWidget *windowanadir;
	GtkWidget *entry1, *entry2, *entry3;
	GtkWidget **entry;
	GtkWidget *boxanadir;

	entry = calloc(0,3*sizeof(GtkWidget));


	GtkWidget *sendButton;

	windowanadir = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	boxanadir = gtk_vbox_new(TRUE,2);
	entry1 = gtk_entry_new();
	entry2 = gtk_entry_new();
	entry3 = gtk_entry_new();
	sendButton = gtk_button_new_with_label("Añadir");



	entry[0] = entry1;
	entry[1] = entry2;
	entry[2] = entry3;

	gtk_box_pack_start ((GtkBox *)boxanadir, entry1, TRUE, TRUE, 2);
	gtk_box_pack_start ((GtkBox *)boxanadir, entry2, TRUE, TRUE, 2);
	gtk_box_pack_start ((GtkBox *)boxanadir, entry3, TRUE, TRUE, 2);
	gtk_box_pack_start ((GtkBox *)boxanadir, sendButton, TRUE, TRUE, 2);

	gtk_container_add((GtkContainer *)windowanadir, boxanadir);

	/*Mandar señal para añadir */

	g_signal_connect((GObject *)sendButton, "clicked", (GCallback)add_car,
		(gpointer)entry);


	gtk_widget_show_all(windowanadir);

}


/* Ventana principal */
int main(int argc, char *argv[])
{	

	con = curso_concesionario_alloc();

	GtkWidget *window;
	GtkWidget *botonanadir, *botonborrar, *botonmodificar, *botonsnprintf;
	GtkWidget *box;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	box = gtk_vbox_new(TRUE,2);

	botonanadir = gtk_button_new_with_label("Añadir coche");
	botonborrar = gtk_button_new_with_label("Borrar coche");
	botonmodificar  = gtk_button_new_with_label("Modificar coche");
	botonsnprintf = gtk_button_new_with_label("Show concesionario");

	gtk_box_pack_start ((GtkBox *)box, botonanadir, TRUE, TRUE, 2);
	gtk_box_pack_start ((GtkBox *)box, botonborrar, TRUE, TRUE, 2);
	gtk_box_pack_start ((GtkBox *)box, botonmodificar, TRUE, TRUE, 2);
	gtk_box_pack_start ((GtkBox *)box, botonsnprintf, TRUE, TRUE, 2);

	gtk_container_add((GtkContainer *)window, box);

	g_signal_connect((GObject *)botonanadir, "clicked", (GCallback)anadirCoche, NULL);
	g_signal_connect((GObject *)botonmodificar, "clicked", (GCallback)modificarCoche, NULL);
	g_signal_connect((GObject *)botonborrar, "clicked", (GCallback)borrarCoche, NULL);
	g_signal_connect((GObject *)botonsnprintf, "clicked", (GCallback)ide_snprintf, NULL);




	g_signal_connect((GObject *)window, "delete-event",
			(GCallback)gtk_main_quit, NULL);

	gtk_widget_show_all(window);
	gtk_main();
	return 0;

}