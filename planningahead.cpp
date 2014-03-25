SCRATCH OF THE FUTURE WORKINGS OF THE COMPUTE SHADER



Volume * volume;
GLuint volumeTexture, volId, baseID;
createTexture3D(volumeTexture);

computeShader->enable();

glBindImageTexture(volId, volumeTexture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8); //the texture is binded to the
                                                                                  //volId image unit

//Might be somehow useful
///glActiveTexture(GL_TEXTURE_alfa); //where 'alfa' is the index of the active texture unit

baseID = volume->bindVolumeTexture(); //this function returns the ID where the texture is bound to

computeShader->setUniform("volumeTexture", volId);
computeShader->setUniform("baseTexture", baseID);

glDispatchCompute(volume->getTextureResolution[0], ...[1], ...[2]);

//then, UNBIND volId and baseID
///glBindImageTexture(volID, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
///volume->unbindVolumeTexture();

computeShader->disable();

//from now on, when rendering, use the volume texture stored on the GLuint volumeTexture;
//maybe the volume could substitute the volume texture ID, to make this more organized...

//another example: http://wili.cc/blog/opengl-cs.html
