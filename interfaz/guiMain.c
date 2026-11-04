#include <gtk/gtk.h>
#include "ventana.h"


static void cargarEstilos(void){
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "assets/estilo.css");

    gtk_style_context_add_provider_for_display(gdk_display_get_default(),GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
}


static void activar(GtkApplication *app, gpointer datos){

    cargarEstilos();

    GtkWidget *ventana = crearVentana(app);

    gtk_window_present(GTK_WINDOW(ventana));

}


int main(int argc, char **argv){
    GtkApplication *app;

    app = gtk_application_new(
        "com.gestor.archivos", 
        G_APPLICATION_DEFAULT_FLAGS
    );

    g_signal_connect(app, "activate", G_CALLBACK(activar), NULL);

    int estado = g_application_run(
        G_APPLICATION(app), argc, argv
    );

    g_object_unref(app);

    return estado;
}