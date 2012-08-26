

Using png images as inline resources with gtkmm
_______________________________________________


[1] Convert the png files to C-headers. Example command:
	gdk-pixbuf-csource --raw --name=myimage myimage.png > myimage.h

[2] #Include "required headers".

[3] To go from header to image using gtkmm:

	Glib::RefPtr<Gdk::Pixbuf> ptr;
	Gtk::Image img;

	ptr = Gdk::Pixbuf::create_from_inline(-1, myimage, false); // 'true' if you want to modify the pixbuf
        img.set(ptr);
