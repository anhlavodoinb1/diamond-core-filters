
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <assert.h>
#include "queue.h"
#include "rgb.h"
#include "histo.h"
#include "image_common.h"
#include "img_search.h"
#include "gui_thread.h"
#include "snapfind.h"
#include "fil_histo.h"

#define	MAX_DISPLAY_NAME	64

rgb_histo_search::rgb_histo_search(const char *name, char *descr)
	: example_search(name, descr)
{
	metric = 0;
	simularity = 0.8;
	bins = 3;
	edit_window = NULL;

}

rgb_histo_search::~rgb_histo_search()
{
	printf("rgb_destruct \n");
	return;
}

void
rgb_histo_search::set_bins(int new_bins)
{
	/* XXX any bounds checks */
	bins = new_bins;
	return;
}


void
rgb_histo_search::set_simularity(char *data)
{
	simularity = atof(data);
	if (simularity < 0) {
		simularity = 0.0;
	} else if (simularity > 1.0) {
		simularity = 1.0;
	}
	return;
}

void
rgb_histo_search::set_simularity(double sim)
{
	simularity = sim;
	if (simularity < 0) {
		simularity = 0.0;
	} else if (simularity > 1.0) {
		simularity = 1.0;
	}
	return;
}

int
rgb_histo_search::handle_config(config_types_t conf_type, char *data)
{
	int	err;	
	
	switch (conf_type) {
		case METRIC_TOK:
			set_simularity(data);
			err = 0;
			break;

		default:
			err = example_search::handle_config(conf_type, data);
			break;
	}
	return(err);
}

static void 
cb_update_menu_select(GtkWidget* item, GtkUpdateType  policy)
{
    printf("policy %d \n", policy); 
} 


static GtkWidget *
create_slider_entry(char *name, float min, float max, int dec, float initial,
		float step, GtkObject **adjp)
{
	GtkWidget *container;
	GtkWidget *scale;
	GtkWidget *button;
	GtkWidget *label;
	

	
	container = gtk_hbox_new(FALSE, 10);
	
	label = gtk_label_new(name);
	gtk_box_pack_start(GTK_BOX(container), label, FALSE, FALSE, 0);

	if (max <= 1.0) {
		max += 0.1;
		*adjp = gtk_adjustment_new(min, min, max, step, 0.1, 0.1);
	} else if (max < 50) {
		max++;
		*adjp = gtk_adjustment_new(min, min, max, step, 1.0, 1.0);
	} else {
		max+= 10;
		*adjp = gtk_adjustment_new(min, min, max, step, 10.0, 10.0);
	}
	gtk_adjustment_set_value(GTK_ADJUSTMENT(*adjp), initial);
	
	scale = gtk_hscale_new(GTK_ADJUSTMENT(*adjp));
    gtk_widget_set_size_request (GTK_WIDGET(scale), 200, -1);
    gtk_range_set_update_policy (GTK_RANGE(scale), GTK_UPDATE_CONTINUOUS);
    gtk_scale_set_draw_value (GTK_SCALE(scale), FALSE);
    gtk_box_pack_start (GTK_BOX(container), scale, TRUE, TRUE, 0);
    gtk_widget_set_size_request(scale, 120, 0);

  	button = gtk_spin_button_new(GTK_ADJUSTMENT(*adjp), step, dec);
    gtk_box_pack_start(GTK_BOX(container), button, FALSE, FALSE, 0);
					
	gtk_widget_show(container);
	gtk_widget_show(label);
	gtk_widget_show(scale);
	gtk_widget_show(button);
									
	return(container);
}

GtkWidget *
make_menu_item (gchar* name, GCallback callback, gpointer  data) 
{
	GtkWidget *item; 

	item = gtk_menu_item_new_with_label(name); 
	g_signal_connect(G_OBJECT(item), "activate", callback, (gpointer) data);
	gtk_widget_show(item); 

	return item; 
}

static void
cb_close_edit_window(GtkWidget* item, gpointer data)
{
	rgb_histo_search *	search;

	search = (rgb_histo_search *)data;
	search->close_edit_win();
}


void
rgb_histo_search::close_edit_win()
{

	printf("close edit window \n");

	/* save any changes from the edit windows */
	save_edits();

	/* call the parent class to give them change to cleanup */	
	example_search::close_edit_win();

	edit_window = NULL;

}


static void
edit_search_done_cb(GtkButton *item, gpointer data)
{
    GtkWidget * widget = (GtkWidget *)data;
	GUI_CALLBACK_ENTER();
    gtk_widget_destroy(widget);
	GUI_CALLBACK_LEAVE();
}
                                                                                 


void
rgb_histo_search::edit_search()
{
	GtkWidget * widget;
	GtkWidget * box;
	GtkWidget * hbox;
	GtkWidget * opt;
	GtkWidget * item;
	GtkWidget * frame;
	GtkWidget * container;
	GtkWidget * menu;
	char		name[MAX_DISPLAY_NAME];

	/*
	 * see if it already exists, if so raise to top and return.
	 */
	if (edit_window != NULL) {
		gdk_window_raise(GTK_WIDGET(edit_window)->window);
		return;
	}

	edit_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	snprintf(name, MAX_DISPLAY_NAME - 1, "Edit %s", get_name());
	name[MAX_DISPLAY_NAME -1] = '\0';
	gtk_window_set_title(GTK_WINDOW(edit_window), name);
	g_signal_connect(G_OBJECT(edit_window), "destroy",
		  	G_CALLBACK(cb_close_edit_window), this);
	box = gtk_vbox_new(FALSE, 10);

	/* create button to close the edit window */
    hbox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, TRUE, 0);
   
    widget = gtk_button_new_with_label("Close");
	g_signal_connect(G_OBJECT(widget), "clicked",
			 	G_CALLBACK(edit_search_done_cb), edit_window);
	GTK_WIDGET_SET_FLAGS(widget, GTK_CAN_DEFAULT);
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, TRUE, 0);

    /*
     * Get the controls from the img_search.
     */
    widget = img_search_display();
    gtk_box_pack_start(GTK_BOX(box), widget, FALSE, TRUE, 0);


	/*
  	 * Create the histo parameters.
	 */

    frame = gtk_frame_new("RGB Histo Params");
    container = gtk_vbox_new(FALSE, 10);
    gtk_container_add(GTK_CONTAINER(frame), container);
                                                                                
    widget = create_slider_entry("Min Simularity", 0.0, 1.0, 2,
        simularity, 0.05, &sim_adj);
    gtk_box_pack_start(GTK_BOX(container), widget, FALSE, TRUE, 0);

    widget = create_slider_entry("Bins", 1, 20, 0,
        bins, 1, &bin_adj);
    gtk_box_pack_start(GTK_BOX(container), widget, FALSE, TRUE, 0);


	opt = gtk_option_menu_new();
	menu = gtk_menu_new();
 
	item = make_menu_item("L1 Distance", G_CALLBACK(cb_update_menu_select),
			GINT_TO_POINTER(0));
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item); 

	item = make_menu_item ("L2 Distance", G_CALLBACK (cb_update_menu_select),
			GINT_TO_POINTER(1));
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item); 

	item = make_menu_item ("Earth Movers", G_CALLBACK(cb_update_menu_select),
			GINT_TO_POINTER(2));
	gtk_menu_shell_append (GTK_MENU_SHELL(menu), item); 

	gtk_option_menu_set_menu(GTK_OPTION_MENU (opt), menu);
	gtk_box_pack_start(GTK_BOX(container), opt, FALSE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(box), frame, FALSE, TRUE, 0);


	/*
	 * Get the controls from the window search class.
 	 */ 
	widget = get_window_cntrl();
	gtk_box_pack_start(GTK_BOX(box), widget, FALSE, TRUE, 0);

	widget = example_display();
	gtk_box_pack_start(GTK_BOX(box), widget, TRUE, TRUE, 0);


	gtk_container_add(GTK_CONTAINER(edit_window), box); 

	//gtk_window_set_default_size(GTK_WINDOW(edit_window), 400, 500);
	gtk_widget_show_all(edit_window);

}

/*
 * This method reads the values from the current edit
 * window if there is an active one.
 */

void
rgb_histo_search::save_edits()
{
	int		bins;
	double	sim;

	/* no active edit window, so return */
	if (edit_window == NULL) {
		return;
	}

	/* get the number of bins and save */
	bins = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(bin_adj));
	set_bins(bins);

	/* get the simularity and save */
	sim = gtk_adjustment_get_value(GTK_ADJUSTMENT(sim_adj));
	set_simularity(sim);

	/* call the parent class */	
	example_search::save_edits();
}

/*
 * This write the relevant section of the filter specification file
 * for this search.
 */

void
rgb_histo_search::write_fspec(FILE *ostream)
{
	Histo 	hgram;
	example_patch_t	*cur_patch;
	img_search	*	rgb;
	int		i = 0;

	save_edits();

	/*
 	 * First we write the header section that corrspons
 	 * to the filter, the filter name, the assocaited functions.
 	 */

	fprintf(ostream, "\n");
	fprintf(ostream, "FILTER %s \n", get_name());
	fprintf(ostream, "THRESHOLD %d \n", 1);	/* XXX should we change */
	fprintf(ostream, "EVAL_FUNCTION  f_eval_histo_detect \n");
	fprintf(ostream, "INIT_FUNCTION  f_init_histo_detect \n");
	fprintf(ostream, "FINI_FUNCTION  f_fini_histo_detect \n");
	fprintf(ostream, "ARG  %s  # name \n", get_name());
	


	/*
	 * Next we write call the parent to write out the releated args,
	 * not that since the args are passed as a vector of strings
	 * we need keep the order the args are written constant or silly
	 * things will happen.
	 */
	example_search::write_fspec(ostream);

	/*
	 * Now write the state needed that is just dependant on the histogram
	 * search.  This will have the histo releated parameters
	 * as well as the linearized histograms.
	 */

	fprintf(ostream, "ARG  %d  # num bins \n", HBINS);
	fprintf(ostream, "ARG  %f  # simularity \n", simularity);
	fprintf(ostream, "ARG  %d  # distance metric \n", metric);
	fprintf(ostream, "ARG  %d  # num examples \n", num_patches);

	//printf("XXXX write patch \n");

	TAILQ_FOREACH(cur_patch, &ex_plist, link) {
		int	bins;
		histo_fill_from_subimage(&hgram, cur_patch->patch_image, 
			0, 0, cur_patch->xsize, cur_patch->ysize);
		normalize_histo(&hgram);
		for (bins=0; bins < (HBINS * HBINS * HBINS); bins++) {
	 		fprintf(ostream, "ARG  %f  # sample %d val %d \n", 
				hgram.data[bins], i, bins);
		}
		i++;
	}
	fprintf(ostream, "REQUIRES  RGB  # dependancies \n");
	fprintf(ostream, "\n");

	rgb = new rgb_img("RGB image", "RGB image");
	ss_add_dep(rgb);
}

void
rgb_histo_search::write_config(FILE *ostream, const char *dirname)
{

	save_edits();

	/* create the search configuration */
	fprintf(ostream, "\n\n");
	fprintf(ostream, "SEARCH rgb_histogram %s\n", get_name());

	/* write out the rgb parameters */
	fprintf(ostream, "METRIC %f \n", simularity);
	/* XXX add bins XXX */

	example_search::write_config(ostream, dirname);

	return;
}

void
rgb_histo_search::region_match(RGBImage *img, bbox_list_t *blist)
{
	histo_config_t	hconfig;
	patch_t		*	hpatch;
	example_patch_t* epatch;
	int				i;
	int				pass;

	save_edits();

	hconfig.name = strdup(get_name());
	assert(hconfig.name != NULL);

	hconfig.req_matches = 1;	/* XXX */
	hconfig.scale = get_scale();
	hconfig.xsize = get_testx();
	hconfig.ysize = get_testy();
	hconfig.stride = get_stride();
	hconfig.bins = HBINS;	/* XXX */ 
	hconfig.simularity = simularity;
	hconfig.distance_type = metric;

	TAILQ_INIT(&hconfig.patchlist);

	i = 0;
	TAILQ_FOREACH(epatch, &ex_plist, link) {
		hpatch = (patch_t *) malloc(sizeof(*hpatch));
		histo_clear(&hpatch->histo);
		assert(hpatch != NULL);
			
		histo_fill_from_subimage(&hpatch->histo, epatch ->patch_image, 
			0, 0,  epatch->xsize, epatch->ysize);
		normalize_histo(&hpatch->histo);
		hpatch->histo.weight = 1.0;

		TAILQ_INSERT_TAIL(&hconfig.patchlist, hpatch, link);
		i++;
	}
	hconfig.num_patches = i;


	pass =  histo_scan_image(hconfig.name, img, NULL, &hconfig, 
		INT_MAX /* XXX */, blist);
    

	                                                                                 
	/* XXX cleanup */

	return;
}
