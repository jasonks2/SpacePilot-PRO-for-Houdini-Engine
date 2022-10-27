import time
import hou
import numpy
import toolutils
import numpy


camera_paths = [] #many cameras in the scene

def pilotTest():
    
    desktop = hou.ui.curDesktop()
    scene_viewer = desktop.paneTabOfType(hou.paneTabType.SceneViewer)
    viewport = scene_viewer.curViewport()
   
    cam = hou.node("/obj/cam1")
    viewport.setCamera(cam)
    camera = viewport.defaultCamera() #SAVE CAMERA FOR later
    camera_default = camera.stash()
    pipein = hou.node("/ch/ch1/pipein1")
    raw_input = []
    raw_input = [track.eval() for track in pipein.tracks()]
    
    if pipein.track("button") is not None:
        if (pipein.track("button").eval() == 12):
            #origin = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]] 
            #input = numpy.array(raw_input)
            #set camera default position when button(12) is pressed
            viewport.setDefaultCamera(camera_default)
            old_cam = viewport.camera()
            raw_input[6] += 1

    if any([v != 0 for v in raw_input]):
        t = []
        input = numpy.array(raw_input)
        
        flip_sign = -1
        #flipped = numpy.multiply(input[-1], flip_sign) #flips z axis
        
        t = numpy.multiply(input[0:3], 0.1) #scalar
        
        #t[0] *= flip_sign
        #t[1] *= flip_sign
        t[2] *= flip_sign #only flip z axis
        
        r = numpy.multiply(input[3:6], 0.01)
        #r[0] *= flip_sign
        #r[1] *= flip_sign
        r[2] *= flip_sign # only flip z axis
      
        print(t)
        
        t = hou.hmath.buildTransform({"translate": t, "rotate": r})
        m = viewport.viewTransform()
        print(m)
        
        new_port = t * m
    

        old_cam = viewport.camera()
        
        
        new_cam = old_cam.setParmTransform(new_port)
        
        cam_obj = hou.node("/ob/cam1")
        parm_t = cam_obj.parmTuple('t')
        parm_t.setKeyFrame(hou.BaseKeyframe(1))
        
        
        #old_cam.draw()
        
        #print(p, flush=True)
    
            
        #raw_input = numpy.array(input).reshape(3,2)
       
    pipein.cook(force=True)
    
    
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
    
    

callback = hou.ui.addEventLoopCallback(checkForAndProcessEvent)
