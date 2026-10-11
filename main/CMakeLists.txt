# Application: wires the lights, led, button and siren components together.
idf_component_register(SRCS "main.c"
                       PRIV_REQUIRES lights led button siren)
