import time
import hou
import numpy
import toolutils
import numpy


camera_paths = [] #many cameras in the scene
desktop = hou.ui.curDesktop()
scene_viewer = desktop.paneTabOfType(hou.paneTabType.SceneViewer)
viewport = scene_viewer.curViewport()
   
cam = hou.node("/obj/cam1")
viewport.setCamera(cam)
camera = viewport.defaultCamera() #SAVE CAMERA FOR later
camera_default = camera.stash()
pipein = hou.node("/ch/ch1/pipein1")
start_time_seconds = 0
stop_time_seconds = 0

def pilotTest():

    raw_input = []
    raw_input = [track.eval() for track in pipein.tracks()]
    global start_time_seconds 
    global stop_time_seconds
    if pipein.track("button") is not None:
        if (pipein.track("button").eval() == 26):
            state = 1
            reset_translate = [-1055.22,1032.18,1259.79]
            reset_rotation = [533.738,242.524,-188.386]
            
            build_reset = t = hou.hmath.buildTransform({"translate": reset_translate, "rotate": reset_rotation})
            
            
            viewport.lockCameraToView(False)
            origin = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]]
           
            
            reset_pos = hou.hmath.buildTranslate((0,0,0))
            current_pos = viewport.viewTransform()
            new_pos =  reset_pos * current_pos
            
            input = numpy.array(raw_input)
            #set camera default position when button(12) is pressed
            #viewport.setDefaultCamera(camera_default)
            old_cam = viewport.camera()
            if old_cam is not None:
                old_cam.setParmTransform(build_reset).draw()
            raw_input[6] += 1
    
            
    
    if any([v != 0 for v in raw_input]):
        t = []
        input = numpy.array(raw_input)
        
        flip_sign = -1
        
        t = numpy.multiply(input[0:3], 0.1) #scalar
        
        #t[0] *= flip_sign
        #t[1] *= flip_sign
        t[2] *= flip_sign
        
        r = numpy.multiply(input[3:6], 0.01)
        #r[0] *= flip_sign
        #r[1] *= flip_sign
        r[2] *= flip_sign
      
        #print(t)
        
        t = hou.hmath.buildTransform({"translate": t, "rotate": r})
        m = viewport.viewTransform()
        
        new_port = t * m
    
        #viewport.lockCameraToView(True)
        old_cam = viewport.camera()
        
        if old_cam is not None:
            new_cam = old_cam.setParmTransform(new_port)
    
    if pipein.track("button") is not None:
        if (pipein.track("button").eval() == 23):
            if (hou.playbar.isPlaying() == False):
                hou.playbar.play()
                start_time_seconds = hou.frame() / hou.fps()
                print("Started recording at time: " + str(start_time_seconds))
                print(hou.time())
                
    if pipein.track("button") is not None:
        if (pipein.track("button").eval() == 0):
            if (hou.playbar.isPlaying() == True):
                hou.playbar.stop()
                
                stop_time_seconds = hou.frame() / hou.fps()
                print("Stopped recording at time: " + str(stop_time_seconds))
                print(hou.time())
                print("Total duration: " + (stop_time_seconds - start_time_seconds))
    pipein.cook(force=True)
    
    
def checkForAndProcessEvent():
    pilotTest()
    #hou.ui.triggerUpdate()
    pass
    #hou.pwd().hm().pilotTest(200, hou.pwd())
    
def getCamera():
    desktop = hou.ui.curDesktop()
    scene_viewer = desktop.paneTabOfType(hou.paneTabType.SceneViewer)
    viewport = scene_viewer.curViewport()
    cam_obj = viewport.setCamera(hou.node("/obj/cam1"))
    

def outputPlaybarEvent(event_type, frame):
        # Code here.
        current_time = hou.time()
        scheduled_time = current_time + 1.5
        if event_type == hou.playbarEvent.FrameChanged:
            t = hou.parmTuple('/obj/cam1/t')
            t.lock((False,False,False))
            #t.deleteKeyframeAtFrame(hou.frame())
            
            for translations in t:
                key = hou.Keyframe()
                key.setFrame(hou.frame())
                t_value = translations.eval()
                key.setValue(t_value)
                t[translations].setKeyframe(key)
            
                
            r = hou.parmTuple('/obj/cam1/r')
            r.lock((False,False,False))
            #r.deleteKeyframeAtFrame(hou.frame())
            
            for rotations in r:
                rkey = hou.Keyframe()
                rkey.setFrame(hou.frame())
                r_value = rotations.eval()
                rkey.setValue(r_value)
                r[rotations].setKeyframe(rkey)
                  
            r = hou.parmTuple('/obj/cam1/r')

            key = hou.KeyFrame()
            key.setFrame(hou.frame())
            key.setValue(tx)
            t[0].setKeyframe(key)
            
            print("Time is: " + str(hou.time()))
        #print ("Playbar event", event_type, "at frame", frame)

playbar_callback = hou.playbar.addEventCallback(outputPlaybarEvent)
callback = hou.ui.addEventLoopCallback(checkForAndProcessEvent)
