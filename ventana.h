#include <gtk/gtk.h>


static void activar(GtkApplication *app, gpointer datos) {
    GtkWidget *ventana;

    ventana = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(ventana), "Mi primera ventana GTK");
    gtk_window_set_default_size(GTK_WINDOW(ventana), 500, 300);

    gtk_window_present(GTK_WINDOW(ventana));
}

/*int main(int argc, char **argv) {
    GtkApplication *app;
    int resultado;

    app = gtk_application_new(
        "com.ejemplo.prueb.gtk",
        G_APPLICATION_DEFAULT_FLAGS
    );

    g_signal_connect(app, "activate", G_CALLBACK(activar), NULL);

    resultado = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return resultado;
}*/

