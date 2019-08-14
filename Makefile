all:
	$(MAKE) -C "payload_generator"
	$(MAKE) -C "haxchi_installer"

clean:
	$(MAKE) -C "payload_generator" clean
	$(MAKE) -C "haxchi_installer" clean