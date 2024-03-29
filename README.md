# NanoRender

Own version of TinyRenderer by **ssloy** --> [Link](https://github.com/ssloy/tinyrenderer)

Some parts of this projected were written using **ssloy**'s code as a guide.

Thank you to **PaulBourke** at [http://paulbourke.net/](http://paulbourke.net/) for the descriptions on how the **[TGA](http://paulbourke.net/dataformats/tga/)** and **[OBJ](http://paulbourke.net/dataformats/obj/)** file formats work. Additionally, the [SPEC](https://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf) sheet for .TGA files is useful for implementing RLE decoding.

Project's current state: **Got model rendering with triangles. Colours are based on how each triangle reflects colour**

Currently working on z-buffer tutorial
Wireframe rendering

![](samples/wireframe.png)

Flat Shading Triangles (Thanks [@rajvirsamrai](https://github.com/rajvirsamrai) for the colour scheme suggestion)

![](samples/flat_red.png)

Simple Intensity Shading

![](samples/intensity_mapped.png)

Textured Model with Intensity Shading in Orthographic Projection

![](samples/texture_mapped_ortho.png)

Textured Model with Intensity Shading in Perspective Projection

![](samples/texture_mapped_proj.png)

Textured Model with Repositioned Camera

![](samples/texture_mapped_with_camera.png)
