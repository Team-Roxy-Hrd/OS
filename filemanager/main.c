#include <gtk/gtk.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

GtkWidget *list_box;
char current_dir[PATH_MAX];

void show_message(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void list_directory(const char *path) {
    gtk_container_foreach(GTK_CONTAINER(list_box), (GtkCallback)gtk_widget_destroy, NULL);

    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(entry->d_name);
        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_widget_set_hexpand(row, TRUE);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
    }

    closedir(dir);
    gtk_widget_show_all(list_box);
    strncpy(current_dir, path, sizeof(current_dir));
}

GtkWidget* get_selected_file() {
    GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box));
    if (!row) return NULL;
    return gtk_bin_get_child(GTK_BIN(row));
}

void on_refresh_clicked(GtkButton *button, gpointer user_data) {
    list_directory(current_dir);
}

void on_back_clicked(GtkButton *button, gpointer user_data) {
    char *last_slash = strrchr(current_dir, '/');
    if (last_slash && last_slash != current_dir) {
        *last_slash = '\0';
    } else {
        strcpy(current_dir, "/");
    }
    list_directory(current_dir);
}

void on_row_activated(GtkListBox *box, GtkListBoxRow *row, gpointer user_data) {
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));
    const char *name = gtk_label_get_text(GTK_LABEL(label));

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", current_dir, name);

    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        strncpy(current_dir, path, sizeof(current_dir));
        list_directory(current_dir);
    }
}

void on_create_file_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create File", NULL, GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter file name");
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", current_dir, filename);
        FILE *fp = fopen(filepath, "w");
        if (fp) {
            fclose(fp);
            list_directory(current_dir);
        } else {
            show_message("Failed to create file.");
        }
    }

    gtk_widget_destroy(dialog);
}

void on_create_dir_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create Directory", NULL, GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter directory name");
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *dirname = gtk_entry_get_text(GTK_ENTRY(entry));
        char dirpath[PATH_MAX];
        snprintf(dirpath, sizeof(dirpath), "%s/%s", current_dir, dirname);
        if (mkdir(dirpath, 0755) == 0) {
            list_directory(current_dir);
        } else {
            show_message("Failed to create directory.");
        }
    }

    gtk_widget_destroy(dialog);
}

void on_delete_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *label = get_selected_file();
    if (!label) return;

    const char *filename = gtk_label_get_text(GTK_LABEL(label));
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", current_dir, filename);

    if (remove(path) == 0) {
        list_directory(current_dir);
    } else {
        show_message("Failed to delete file or directory.");
    }
}

void on_manual_close_clicked(GtkButton *button, gpointer window) {
    gtk_widget_destroy(GTK_WIDGET(window));
}

void on_man_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Project Manual");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(user_data)); // Optional: link to parent

    // Create a vertical box layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Scrolled text view
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Load manual content
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gchar *contents = NULL;
    gsize length;
    GError *error = NULL;
    if (g_file_get_contents("manual.txt", &contents, &length, &error)) {
        gtk_text_buffer_set_text(buffer, contents, -1);
        g_free(contents);
    } else {
        gtk_text_buffer_set_text(buffer, "Could not load manual.txt", -1);
    }

    // Close button
    GtkWidget *close_button = gtk_button_new_with_label("Close");
    gtk_box_pack_start(GTK_BOX(vbox), close_button, FALSE, FALSE, 5);
    g_signal_connect(close_button, "clicked", G_CALLBACK(on_manual_close_clicked), window);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);
}
void on_chmod_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *label = get_selected_file();
    if (!label) return;

    const char *filename = gtk_label_get_text(GTK_LABEL(label));
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Change Permissions", NULL, GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter permission (e.g., 755)");
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *perm_str = gtk_entry_get_text(GTK_ENTRY(entry));
        mode_t mode = strtol(perm_str, NULL, 8);
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", current_dir, filename);
        if (chmod(path, mode) == 0) {
            show_message("Permissions changed successfully.");
        } else {
            show_message("Failed to change permissions.");
        }
    }

    gtk_widget_destroy(dialog);
}

void on_symlink_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *label = get_selected_file();
    if (!label) return;

    const char *target = gtk_label_get_text(GTK_LABEL(label));
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create Symlink", NULL, GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter symlink name");
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *linkname = gtk_entry_get_text(GTK_ENTRY(entry));
        char target_path[PATH_MAX];
        snprintf(target_path, sizeof(target_path), "%s/%s", current_dir, target);
        char link_path[PATH_MAX];
        snprintf(link_path, sizeof(link_path), "%s/%s", current_dir, linkname);
        if (symlink(target_path, link_path) == 0) {
            list_directory(current_dir);
        } else {
            show_message("Failed to create symbolic link.");
        }
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    getcwd(current_dir, sizeof(current_dir));

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "C File Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *refresh_btn = gtk_button_new_with_label("Refresh");
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), refresh_btn, FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_back_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), back_btn, FALSE, FALSE, 0);

    GtkWidget *create_file_btn = gtk_button_new_with_label("Create File");
    g_signal_connect(create_file_btn, "clicked", G_CALLBACK(on_create_file_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), create_file_btn, FALSE, FALSE, 0);

    GtkWidget *create_dir_btn = gtk_button_new_with_label("Create Directory");
    g_signal_connect(create_dir_btn, "clicked", G_CALLBACK(on_create_dir_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), create_dir_btn, FALSE, FALSE, 0);
	

    GtkWidget *delete_btn = gtk_button_new_with_label("Delete");
    g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), delete_btn, FALSE, FALSE, 0);


    GtkWidget *man_btn = gtk_button_new_with_label("Man");
    g_signal_connect(man_btn, "clicked", G_CALLBACK(on_man_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), man_btn, FALSE,FALSE,0);

    GtkWidget *chmod_btn = gtk_button_new_with_label("Change Permissions");
    g_signal_connect(chmod_btn, "clicked", G_CALLBACK(on_chmod_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), chmod_btn, FALSE, FALSE, 0);

    GtkWidget *symlink_btn = gtk_button_new_with_label("Create Symlink");
    g_signal_connect(symlink_btn, "clicked", G_CALLBACK(on_symlink_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), symlink_btn, FALSE, FALSE, 0);

    list_box = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), list_box, TRUE, TRUE, 0);
    g_signal_connect(list_box, "row-activated", G_CALLBACK(on_row_activated), NULL);

    list_directory(current_dir);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
