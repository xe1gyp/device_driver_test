
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>


void dialog_close_cb(GtkWidget * w, gpointer data)
{
	gtk_widget_destroy(data);
}

void show_dialog(char *title, char *body)
{
	GtkWidget *dialog, *button, *label;

	dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dialog), title);
	gtk_container_border_width(GTK_CONTAINER(dialog), 5);
	label = gtk_label_new(body);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE,
			   TRUE, 0);
	gtk_widget_show(label);

	button = gtk_button_new_with_label(" Close ");
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
			   GTK_SIGNAL_FUNC(dialog_close_cb), dialog);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
			   button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	gtk_widget_show(dialog);
	gtk_widget_grab_focus(button);
}

void about(void)
{
	show_dialog("About gFindChip 0.01", "gFindChip 0.01\n\n\
Copyright (c) 2000 Jonathan Hudson <jrhudson@bigfoot.com>,\n\
Copyright (c) 2000 Dag Brattli <dagb@cs.uit.no>,\n\
All Rights Reserved.\n\n\
This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,\n\
USA.");

}


static GtkItemFactoryEntry menu_items[] = {
	{"/_File", NULL, NULL, 0, "<Branch>"},
	{"/File/Quit", "<control>Q", gtk_main_quit, 0, NULL},
	{"/_Help", NULL, NULL, 0, "<LastBranch>"},
	{"/_Help/About", NULL, about, 0, NULL},
};

void get_main_menu(GtkWidget * window, GtkWidget ** menubar)
{
	int nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]);
	GtkItemFactory *item_factory;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new();

	item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
					    accel_group);

	gtk_item_factory_create_items(item_factory, nmenu_items,
				      menu_items, NULL);

	gtk_accel_group_attach(accel_group, GTK_OBJECT(window));

	if (menubar)
		*menubar =
		    gtk_item_factory_get_widget(item_factory, "<main>");
}

void MakeResultLabel(GtkWidget * table, GtkWidget * button[], int i,
		     char *label)
{

	int x, y, n;
	x = 2 * (i % 3);
	y = i / 3;
	n = i * 2 + 1;
	button[n] = gtk_label_new(label);
	gtk_label_set_justify(GTK_LABEL(button[n]), GTK_JUSTIFY_LEFT);
	gtk_table_attach_defaults(GTK_TABLE(table), button[n], x + 1,
				  x + 2, y, y + 1);
	gtk_widget_show(button[n]);
}

void MakeStaticLabels(GtkWidget * table, GtkWidget * button[])
{
	int i, x, y, n;
	static char *labels[] = { "Chip:", "Model:", "Version:",
		"Control I/O:", "SIR I/O:", "FIR I/O:",
		"IRQ:", "DMA:"
	};

	for (i = 0; i < 8; i++) {
		x = 2 * (i % 3);
		y = i / 3;
		n = i * 2;

		button[n] = gtk_label_new(labels[i]);
		gtk_label_set_justify(GTK_LABEL(button[n]),
				      GTK_JUSTIFY_LEFT);
		gtk_table_attach_defaults(GTK_TABLE(table), button[n], x,
					  x + 1, y, y + 1);
		gtk_widget_show(button[n]);
	}
}




int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *main_vbox;
	GtkWidget *menubar;
	GtkWidget *table;
	GtkWidget *button[16];
	FILE *fp;
	int i = 0;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window), "destroy",
			   GTK_SIGNAL_FUNC(gtk_main_quit), "WM destroy");
	gtk_window_set_title(GTK_WINDOW(window), "gFindChip");

	main_vbox = gtk_vbox_new(FALSE, 1);
	gtk_container_border_width(GTK_CONTAINER(main_vbox), 1);
	gtk_container_add(GTK_CONTAINER(window), main_vbox);
	gtk_widget_show(main_vbox);

	get_main_menu(window, &menubar);
	gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);
	gtk_widget_show(menubar);

	table = gtk_table_new(3, 6, 1);

	gtk_container_set_border_width(GTK_CONTAINER(window), 2);

	gtk_container_add(GTK_CONTAINER(main_vbox), table);

	MakeStaticLabels(table, button);

	if ((fp = popen("findchip", "r"))) {
		char buf[128];
		char *p1, *p0, *p2;

		if (fgets(buf, sizeof(buf), fp)) {
			p0 = strchr(buf, '\n');
			*p0 = 0;
			for (i = 0, p1 = buf; (p2 = strchr(p1, ',')); i++) {
				*p2 = 0;
				if (p1 != p2) {
					MakeResultLabel(table, button, i,
							p1);
				}
				p1 = p2 + 1;
			}
			if (*p1) {
				MakeResultLabel(table, button, i, p1);
			}
		}
		pclose(fp);
	}

	if (i == 0) {
		show_dialog("gFindChip",
			    "Unable to find any known IR chip");
	}

	gtk_widget_show(table);
	gtk_widget_show(window);
	gtk_main();

	return (0);
}
