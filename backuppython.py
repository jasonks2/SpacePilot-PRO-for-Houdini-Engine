##################
# Author: jasonks2
##################

import time
import hou
import numpy
import toolutils
import numpy


camera_paths = [] #many cameras in the scene
pipein_paths = [] #many pipeins in the scene
def makeHDA():
    obj = hou.node("/ch/ch1")

    sub_net = obj.collapseIntoSubnet(obj.allSubChildren())
    hda_name = sub_net.name()
    file_path = '/localhome/jasonks2/Desktop'
    file_path = file_path + '/' + '.hda'
    hda_asset = sub_net.createDigitalAsset(hda_name, file_path)


def pilotTest():

    for p in hou.node('/ch').allSubChildren():
        if p.type().name().startswith("pipein"):
            pipein_paths.append(p.path())
            
    if len(pipein_paths) == 0: #create pipein node
        ch = hou.node("/ch")
        chop_net = ch.createNode("ch")
        pipein = chop_net.createNode("pipein")
    else:
        pipein = hou.node(pipein_paths[0])
    
    desktop = hou.ui.curDesktop()
    scene_viewer = desktop.paneTabOfType(hou.paneTabType.SceneViewer)
    viewport = scene_viewer.curViewport()
    cam_obj = viewport.setCamera(hou.node("/obj/cam1"))
    
    camera = viewport.defaultCamera() #SAVE CAMERA FOR later
    camera_default = camera.stash()
 
    raw_input = []
    raw_input = [track.eval() for track in pipein.tracks()]
    
   
    if (pipein.track("button").eval() == 12):
        #origin = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]] 
        #input = numpy.array(raw_input)
        
        #set camera default position when button(12) is pressed
        viewport.setDefaultCamera(camera_default)
        old_cam = viewport.camera()
        raw_input[6] += 1

        
    if any([v != 0 for v in raw_input]):
 
        input = numpy.array(raw_input)
        t = numpy.multiply(input[0:3], 0.1)
        r = numpy.multiply(input[3:6], 0.01)
        
        t = hou.hmath.buildTransform({"translate": t, "rotate": r})
        m = viewport.viewTransform()
        print(m)
        new_port = t * m
        old_cam = viewport.camera()
        new_cam = old_cam.setParmTransform(new_port)
        
        
        #old_cam.draw()
       
    pipein.cook(force=True)
    
def buildEnvironment():
    #ch = hou.node("/ch")
    #chop_net = ch.createNode("ch")
    #pipein = chop_net.createNode("pipein")
    
    
    desktop = hou.ui.curDesktop()
    scene_viewer = desktop.paneTabOfType(hou.paneTabType.SceneViewer)
    viewport = scene_viewer.curViewport()
    #cam_obj = viewport.setCamera(hou.node("/obj/cam1"))
    
    #get list of cameras
    for c in hou.node('/').allSubChildren():
        if c.type().name().startswith("cam"):
            camera_paths.append(c.path())
            
    if len(camera_paths) == 0:
        cam = obj.createNode('cam','asset_cam')
        camera_paths.append(cam.path())
        
    if viewport.camera() > 0:
        index = camera_paths.index(viewport.camera().path()) #return index of current viewport cam given campaths
        maximum = len(camera_paths)
        index2 = (index+1)%maximum
        cam_obj = viewport.setCamera(hou.node(camera_paths[index2]))
        
    else:
        cam_obj = viewport.setCamera(hou.node(camera_paths[0]))
        viewport.lockCameraToView(True)
    
    
def checkForAndProcessEvent():
    pilotTest()
    hou.ui.triggerUpdate()
    pass
    #hou.pwd().hm().pilotTest(200, hou.pwd())



def getCamera():
    desktop = hou.ui.curDesktop()
    scene_viewer = desktop.paneTabOfType(hou.paneTabType.SceneViewer)
    viewport = scene_viewer.curViewport()
    cam_obj = viewport.setCamera(hou.node("/obj/cam1"))
    
    

#callback = hou.ui.addEventLoopCallback(checkForAndProcessEvent)

