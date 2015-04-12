# You should have received this file along a version of NajuEngine, copy it to
# the .blender/<version>/scripts/addons folder.
#
# This file pertains to the Naju engine licencing.

bl_info = {
  "name": "NajuEngine mesh format tools (.mesh)",
  "author": "Tatu Kilappa (original Blender 2.4.x -script by Sami Vuolli)",
  "version": (2, 0, 0),
  "blender": (2, 63, 0),
  "location": "File > Export > NajuEngine mesh (.mesh)",
  "description": "Export tools for NajuEngine mesh format. Additional format-specific utilities included.",
  "wiki_url": "http://wiki.blender.org/index.php/Extensions:2.6/Py/Scripts/Import-Export/",
  "tracker_url": "http://faemiyah.fi/naju",
  "category": "Import-Export"}

"""
This script exports meshes in NajuEngine (.mesh) format. It also includes
utilities for format handling, such as animation preview. These are provided
in the same file both for convenience and due to format handling being
required for proper export.
"""

import bpy
import copy
import mathutils
import os
import re
import sys

BONE_NAME_SEPARATORS = '[_. -]'
LOG_HEADER = "NajuEngine mesh export: "
INDENT = 1
REVISION = 1 # File format revision.

def armature_collect_pose_names(obj, basename = ".*"):
  """Collect poses matching basename from pose library."""
  ret = dict()
  for ii in range(0, len(obj.pose_library.pose_markers)):
    frame = Frame(obj.pose_library.pose_markers[ii], ii, basename)
    if frame.animation:
      if not frame.animation in ret.keys():
        ret[frame.animation] = dict()
      ret[frame.animation][frame.time] = frame
  return ret

def armature_save(xmlfile, scene, obj, sub = False):
  poses = armature_collect_pose_names(obj)

  xmlfile.printLine('<name>%s</name>' % obj.name)

  for ii in obj.data.bones:
    if not ii.parent:
      bone_write(xmlfile, ii)

  for ii, animation in poses.items():
    xmlfile.printLine("<animation>", INDENT)
    xmlfile.printLine("<name>%s</name>" % ii)
    for jj, kk in animation.items():
      if None != kk.refer:
        src = animation[kk.refer]
        print("%swriting frame '%s' (mirror of frame '%s')" % (LOG_HEADER, kk.getName(), src.getName()))
        bpy.ops.poselib.apply_pose(pose_index=src.index)
        pose_mirror(obj.pose.bones)
      else:
        print("%swriting frame '%s'" % (LOG_HEADER, kk.getName()))
        bpy.ops.poselib.apply_pose(pose_index=kk.index)
      xmlfile.printLine("<frame>", INDENT)
      xmlfile.printLine("<time>%s</time>" % jj)
      pose_write(xmlfile, obj.pose.bones)
      xmlfile.printLine("</frame>", -INDENT)
    xmlfile.printLine("</animation>", -INDENT)

def bone_write(xmlfile, bone):
  """Write one bone to file."""
  xmlfile.printLine("<bone>", INDENT)
  xmlfile.printLine('<name>%s</name>' % bone.name)
  xmlfile.printLine('<tail>', INDENT)
  xmlfile.printLine('<x>%f</x>' % bone.tail.x)
  xmlfile.printLine('<y>%f</y>' % bone.tail.y)
  xmlfile.printLine('<z>%f</z>' % bone.tail.z)
  xmlfile.printLine('</tail>', -INDENT)
  xmlfile.printLine('<head>', INDENT)
  xmlfile.printLine('<x>%f</x>' % bone.head.x)
  xmlfile.printLine('<y>%f</y>' % bone.head.y)
  xmlfile.printLine('<z>%f</z>' % bone.head.z)
  xmlfile.printLine('</head>', -INDENT)
  for ii in bone.children:
    bone_write(xmlfile, ii)
  xmlfile.printLine("</bone>", -INDENT)

def face_get_image(face):
  """Get image referenced by given face."""
  texture = bpy.data.materials[face.material_index].active_texture
  if texture and ('IMAGE' == texture.type):
    return texture.image
  return None

def mesh_create_face_vertex_key(mesh, fidx, vidx):
  """Create a fictionary key for given vertex in given face."""
  face = mesh.polygons[fidx]
  image = face_get_image(face)
  if image:
    tessface = mesh_get_tessface(mesh, fidx)
    return (face.vertices[vidx], face.material_index, tessface.uv[vidx][0],tessface.uv[vidx][1])
  return (face.vertices[vidx], face.material_index)

def mesh_get_tessface(mesh, fidx):
  return mesh.tessface_uv_textures.active.data.values()[fidx]

def mesh_get_vertex(mesh, vidx):
  return mesh.vertices[vidx]

def mesh_collect_images(obj):
  """Collect all distinct images from an object."""
  ret = []
  for face in obj.data.polygons:
    image = face_get_image(face)
    if image and (not image in ret):
      ret += [image]
  if 0 >= len(ret):
    return [None]
  return ret

def mesh_save(xmlfile, scene, obj, image, sub = False):
  """Saves an individual mesh using given texture."""
  mesh = obj.to_mesh(scene, True, 'PREVIEW')
  numverts = 0
  vdict = {}

  xmlfile.printLine('<name>%s</name>' % obj.name)

  if sub:
    xmlfile.printLine('<offset>', INDENT)
    xmlfile.printLine('<x>%f</x>' % (obj.location.x))
    xmlfile.printLine('<y>%f</y>' % (obj.location.y))
    xmlfile.printLine('<z>%f</z>' % (obj.location.z))
    xmlfile.printLine('</offset>', -INDENT)

  for ii in range(len(mesh.polygons)):
    face = mesh.polygons[ii]
    if face_get_image(face) != image:
      continue
    mat = mesh.materials[face.material_index]
    for jj in range(len(face.vertices)):
      key = mesh_create_face_vertex_key(mesh, ii, jj)
      if not key in vdict:
        vdict[key] = numverts
        numverts += 1
        xmlfile.printLine("<vertex>", INDENT)
        xmlfile.printLine("<color>", INDENT)
        xmlfile.printLine("<r>%f</r>" % (mat.diffuse_color.r))
        xmlfile.printLine("<g>%f</g>" % (mat.diffuse_color.g))
        xmlfile.printLine("<b>%f</b>" % (mat.diffuse_color.b))
        xmlfile.printLine("<a>%f</a>" % (1.0 - mat.translucency.real))
        xmlfile.printLine("</color>", -INDENT)
        if image:
          tessface = mesh_get_tessface(mesh, ii)
          xmlfile.printLine("<texcoord>", INDENT)
          xmlfile.printLine("<s>%f</s>" % (tessface.uv[jj][0]))
          xmlfile.printLine("<t>%f</t>" % (tessface.uv[jj][1]))
          xmlfile.printLine("</texcoord>", -INDENT)
        ver = mesh_get_vertex(mesh, face.vertices[jj])
        xmlfile.printLine("<x>%f</x>" % (ver.co.x))
        xmlfile.printLine("<y>%f</y>" % (ver.co.y))
        xmlfile.printLine("<z>%f</z>" % (ver.co.z))
        xmlfile.printLine("</vertex>", -INDENT)

  for ii in range(len(mesh.polygons)):
    face = mesh.polygons[ii]
    if face_get_image(face) != image:
      continue
    xmlfile.printLine("<face>", INDENT)
    i1 = vdict[mesh_create_face_vertex_key(mesh, ii, 0)]
    i2 = vdict[mesh_create_face_vertex_key(mesh, ii, 1)]
    i3 = vdict[mesh_create_face_vertex_key(mesh, ii, 2)]
    xmlfile.printLine("<a>%i</a>" % (i1))
    xmlfile.printLine("<b>%i</b>" % (i2))
    xmlfile.printLine("<c>%i</c>" % (i3))
    if len(face.vertices) == 4:
      i4 = vdict[mesh_create_face_vertex_key(mesh, ii, 3)]
      xmlfile.printLine("</face>", -INDENT)
      xmlfile.printLine("<face>", INDENT)
      xmlfile.printLine("<a>%i</a>" % (i1))
      xmlfile.printLine("<b>%i</b>" % (i3))
      xmlfile.printLine("<c>%i</c>" % (i4))
    xmlfile.printLine("</face>", -INDENT)
 
  tpath = "gfx/textures"
  if image:
    tex = os.path.basename(image.filepath)
  else:
    tex = "stub.png"
  xmlfile.printLine("<texture>%s/%s</texture>" % (tpath, tex))

def object_list_save(filename, scene, objlst, subidx = None):
  """Saves a collection of objects."""

  armatures = []
  meshes = []

  for ii in objlst:
    if 'ARMATURE' == ii.type:
      armatures += [ii]
    elif 'MESH' == ii.type:
      meshes += [ii]
    else:
      print("%snot going to save object of type '%s'" % (LOG_HEADER, obj.type))

  if 0 < len(meshes):
    obj = meshes[0]
    images = mesh_collect_images(obj)
    if (1 < len(images)) or (1 < len(objlst)):
      metafile = XmlFile(filename + ".mesh", "meta-mesh")
      metafile.printLine("<revision>%i</revision>" % (REVISION))
      metafile.printLine("<scale>", INDENT)
      metafile.printLine("<center>0</center>")
      metafile.printLine("<into>0</into>")
      metafile.printLine("<x>1.0</x>")
      metafile.printLine("<y>1.0</y>")
      metafile.printLine("<z>1.0</z>")
      metafile.printLine("</scale>", -INDENT)
      for ii in range(len(images)):
        print("%sroot mesh: '%s'" % (LOG_HEADER, obj.name))
        metafile.printLine("<mesh>", INDENT)
        mesh_save(metafile, scene, obj, images[ii])
        metafile.printLine("<mesh>", -INDENT)
      for ii in range(1, len(meshes)):
        obj = objlst[ii]
        images = mesh_collect_images(obj)
        for jj in range(len(images)):
          print("%ssubmesh: '%s'" % (LOG_HEADER, obj.name))
          metafile.printLine("<submesh>", INDENT)
          mesh_save(metafile, scene, obj, images[jj], True)
          metafile.printLine("<submesh>", -INDENT)
      metafile.close()
    else: # only one mesh
      xmlfile = XmlFile(filename + ".mesh", "mesh")
      xmlfile.printLine("<revision>%i</revision>" % (REVISION))
      xmlfile.printLine("<scale>", INDENT)
      xmlfile.printLine("<center>0</center>")
      xmlfile.printLine("<into>0</into>")
      xmlfile.printLine("<x>1.0</x>")
      xmlfile.printLine("<y>1.0</y>")
      xmlfile.printLine("<z>1.0</z>")
      xmlfile.printLine("</scale>", -INDENT)
      print("%smesh: '%s'" % (LOG_HEADER, obj.name))
      mesh_save(xmlfile, scene, obj, images[0])
      xmlfile.close()

  if 0 < len(armatures):
    obj = armatures[0]
    if 1 < len(armatures):
      metafile  = XmlFile(filename + ".armature", "meta-armature")
      metafile.printLine("<revision>%i</revision>" % (REVISION))
      metafile.printLine("<scale>", INDENT)
      metafile.printLine("<center>0</center>")
      metafile.printLine("<into>0</into>")
      metafile.printLine("<x>1.0</x>")
      metafile.printLine("<y>1.0</y>")
      metafile.printLine("<z>1.0</z>")
      metafile.printLine("</scale>", -INDENT)
      # Save root armature.
      print("%sroot armature: '%s'" % (LOG_HEADER, obj.name))
      metafile.printLine("<armature>", INDENT)
      armature_save(metafile, scene, obj, images[ii])
      metafile.printLine("<armature>", -INDENT)
      # Save child armatures.
      for ii in range(1, len(armatures)):
        obj = armatures[ii]
        for jj in range(len(images)):
          print("%ssub-armature: '%s'" % (LOG_HEADER, obj.name))
          metafile.printLine("<sub-armature>", INDENT)
          armature_save(metafile, scene, obj, True)
          metafile.printLine("<sub-armature>", -INDENT)
      metafile.close()
    else:
      xmlfile = XmlFile(filename + ".armature", "armature")
      xmlfile.printLine("<revision>%i</revision>" % (REVISION))
      xmlfile.printLine("<scale>", INDENT)
      xmlfile.printLine("<center>0</center>")
      xmlfile.printLine("<into>0</into>")
      xmlfile.printLine("<x>1.0</x>")
      xmlfile.printLine("<y>1.0</y>")
      xmlfile.printLine("<z>1.0</z>")
      xmlfile.printLine("</scale>", -INDENT)
      print("%sarmature: '%s'" % (LOG_HEADER, obj.name))
      armature_save(xmlfile, scene, obj, None)
      xmlfile.close()

def pose_mirror(lst):
  """Mirror a pose left-right."""
  handled = []
  for ii in lst:
    if ii.name in handled:
      continue
    match = re.match('^(.*)%s([rlRL])$' % BONE_NAME_SEPARATORS, ii.name)
    if not match:
      continue
    base = match.groups(1)[0]
    lhs = match.groups(1)[1]
    if ('l' == lhs) or ('L' == lhs):
      rhs = '[rR]'
    else:
      rhs = '[lL]'
    kk = None
    for jj in lst:
      if not jj.name in handled:
        match = re.match('^%s%s%s$' % (base, BONE_NAME_SEPARATORS, rhs), jj.name)
        if match:
          if kk:
            raise RuntimeError("bone '%s' has more than one counterpart ('%s', '%s')" %
                (ii.name. kk.name, jj.name))
          kk = jj
    if not kk:
      raise RuntimeError("bone '%s' should have a counterpart" % ii.name)
    if ii.rotation_mode != kk.rotation_mode:
      raise RuntimeError("bone '%s' has different rotation mode (%s) from '%s' (%s)'" %
          (ii.name, ii.rotation_mode, kk.name, kk.rotation_mode))
    if ii.rotation_mode == 'QUATERNION':
      quat_lhs = copy.deepcopy(ii.rotation_quaternion)
      quat_rhs = copy.deepcopy(kk.rotation_quaternion)
      ii.rotation_quaternion = quat_rhs
      kk.rotation_quaternion = quat_lhs
    elif ii.rotation_mode == 'XYZ':
      euler_lhs = copy.deepcopy(ii.rotation_euler)
      euler_rhs = copy.deepcopy(kk.rotation_euler)
      ii.rotation_euler = euler_rhs
      kk.rotation_euler = euler_lhs
    else:
      raise RuntimeError("invalid rotation mode '%s'" % ii.rotation_mode)
    if ii.location == mathutils.Vector((0,0,0)):
      if kk.location != mathutils.Vector((0,0,0)):
        raise RuntimeError("bone '%s' has location infomation even though '%s' does not" %
            (kk.name, ii.name))
    else:
      if kk.location == mathutils.Vector((0,0,0)):
        raise RuntimeError("bone '%s' has location infomation even though '%s' does not" %
            (ii.name, kk.name))
      loc_lhs = copy.deepcopy(ii.location)
      loc_rhs = copy.deepcopy(kk.location)
      ii.location = loc_rhs
      kk.location = loc_lhs
    handled += [ii.name, kk.name]

def pose_write(xmlfile, lst):
  """Write a list of bone locations into a file."""
  for ii in lst:
    xmlfile.printLine("<bone>", INDENT)
    xmlfile.printLine("<name>%s</name>" % ii.name)
    xmlfile.printLine("<location>", INDENT)
    xmlfile.printLine("<x>%f</x>" % ii.location.x)
    xmlfile.printLine("<y>%f</y>" % ii.location.y)
    xmlfile.printLine("<z>%f</z>" % ii.location.z)
    xmlfile.printLine("</location>", -INDENT)
    if ii.rotation_mode == 'QUATERNION':
      quat = ii.rotation_euler.to_quaternion()
    else:
      quat = ii.rotation_quaternion
    xmlfile.printLine("<rotation>", INDENT)
    xmlfile.printLine("<w>%f</w>" % quat.w)
    xmlfile.printLine("<x>%f</x>" % quat.x)
    xmlfile.printLine("<y>%f</y>" % quat.y)
    xmlfile.printLine("<z>%f</z>" % quat.z)
    xmlfile.printLine("</rotation>", -INDENT)
    xmlfile.printLine("</bone>", -INDENT)

class Frame:
  """Frame class."""

  def __init__(self, pose, index, basename):
    """Constructor."""
    self.pose = pose
    self.index = index
    self.animation = None
    self.time = None
    self.refer = None
    match = re.match('(%s)\s+(\d+)$' % basename, pose.name)
    if match:
      self.animation = match.groups(1)[0]
      self.time = float(match.groups(1)[1])
    else:
      match = re.match('(%s)\s+(\d+)\s+.*(\d+).*' % basename, pose.name)
      if match:
        self.animation = match.groups(1)[0]
        self.time = float(match.groups(1)[1])
        self.refer = float(match.groups(1)[2])
      else:
        raise RuntimeError("name '%s' can not be interpreted as a frame" % (pose.name))

  def getName(self):
    return self.pose.name

class NajuEngineExportOperator(bpy.types.Operator):
  """Operator for export dialog"""
  bl_idname = "export.naju_engine_mesh"
  bl_label = "Export NajuEngine mesh"
  bl_description = "Exports selected objects in NajuEngine mesh format"
  bl_options = {'REGISTER', 'UNDO'}
  
  filename = bpy.props.StringProperty(name="File Name", description="Name of the file")
  directory = bpy.props.StringProperty(name="Directory", description="Directory of the file")
   
  def execute(self, context):
    """Implementation of execute."""
    fname = self.directory + re.sub(r"\.[^\.]*$", "", self.filename)
    object_list_save(fname, context.scene, context.selected_objects)
    return {'FINISHED'}

  def invoke(self, context, event):
    """Implementation of invoke."""
    wm = bpy.context.window_manager
    wm.fileselect_add(self)
    return {'RUNNING_MODAL'}

bpy.types.Scene.naju_target_animation = bpy.props.StringProperty(
    name = 'naju_target_animation',
    description = 'Name of the pose library to map into the animation timeline',
    maxlen = 32)

class NajuEngineAnimationPanel(bpy.types.Panel):
  """Creates an additional panel in the armature window."""
  bl_label = "Naju Animation Panel"
  bl_idname = "DATA_PT_naju_animation"
  bl_context = "data"
  bl_region_type = "WINDOW"
  bl_space_type = "PROPERTIES"

  @classmethod
  def poll(cls, context):
    if not context.armature:
      return False
    return True

  def draw(self, context):
    layout = self.layout

    #row = layout.row()
    #row.label(text="Active object is: " + context.object.name)

    box = layout.box()
    row = box.row()
    row.prop(context.scene, "naju_target_animation", 'Pose library name')
    row = box.row()
    row.operator('wm.naju_animation', 'Apply')

class NajuEngineApplyAnimationOperator(bpy.types.Operator):
  """Operator for creating animations from poses."""
  bl_idname = "wm.naju_animation"
  bl_label = "Naju Animation Operator"

  def execute(self, context):
    basename = context.scene.naju_target_animation
    if len(basename) <= 0:
      raise RuntimeError("invalid animation basename supplied")
      return {'FINISHED'}
    scene = context.scene
    for ii in range(scene.frame_start, scene.frame_end):
      #obj.keyframe_delete('location', frame = ii)
      bpy.ops.anim.change_frame(frame = int(ii))
      bpy.ops.anim.keyframe_delete_v3d()
    poses = armature_collect_pose_names(context.object, basename)
    if 0 >= len(poses):
      raise RuntimeError("no poses found for basename '%s'" % basename)
      return {'FINISHED'}
    elif 1 < len(poses):
      raise RuntimeError("more than one animation found for basename '%s'" % basename)
      return {'FINISHED'}
    animation = poses[min(poses)]
    for ii in animation.values():
      if None != ii.refer:
        src = animation[ii.refer]
        print("pose '%s' is a mirror of pose '%s'" % (ii.getName(), src.getName()))
        bpy.ops.poselib.apply_pose(pose_index=src.index)
        pose_mirror(context.visible_pose_bones)
        bpy.ops.poselib.pose_remove(pose=ii.getName())
        bpy.ops.poselib.pose_add(frame=ii.index, name=ii.getName())
    scene.frame_start = 0
    scene.frame_end = len(animation) * 5
    idx = 0
    first_pose = None
    for ii in animation.values():
      bpy.ops.anim.change_frame(frame = idx * 5)
      bpy.ops.poselib.apply_pose(pose_index=ii.index)
      bpy.ops.anim.keyframe_insert(type='LocRotScale')
      if not first_pose:
        first_pose = ii
      idx += 1
    bpy.ops.anim.change_frame(frame = idx * 5)
    bpy.ops.poselib.apply_pose(pose_index=first_pose.index)
    bpy.ops.anim.keyframe_insert(type='LocRotScale')
    return {'FINISHED'}

class XmlFile:
  """XML file abstraction."""

  def __init__(self, pfilename, ptype, pindent = INDENT):
    """Constructor, implicitly opens the file."""
    self.filename = pfilename
    self.file = open(pfilename, "wt")
    self.indent = 0
    self.type = ptype
    self.printLine('<?xml version="1.0" encoding="utf-8"?>')
    self.printLine("<" + ptype + ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">', pindent)

  def close(self):
    """Close the opened file."""
    self.printLine("</%s>" % (self.type), -INDENT)
    self.file.close()
    print("%ssaved: '%s'" % (LOG_HEADER, self.filename))

  def printLine(self, line, indent_change = 0):
    """Print one line into the file."""
    if 0 > indent_change: # negative indent before writing line
      self.indent += indent_change
    if self.indent < 0:
      self.indent = 0
    for ii in range(self.indent):
      print("  ", file = self.file, end = "")
    print(line, file = self.file)
    if 0 < indent_change: # positive indent after writing line
      self.indent += indent_change

def menu_func(self, context):
  """Menu callback."""
  self.layout.operator(NajuEngineExportOperator.bl_idname, text="NajuEngine mesh (.mesh)", icon='PLUGIN')

def register():
  """Register module."""
  bpy.utils.register_class(NajuEngineAnimationPanel)
  bpy.utils.register_class(NajuEngineApplyAnimationOperator)
  bpy.utils.register_module(__name__)
  bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
  """Unregister module."""
  bpy.utils.unregister_class(NajuEngineAnimationPanel)
  bpy.utils.unregister_class(NajuEngineApplyAnimationOperator)
  bpy.utils.unregister_module(__name__)
  bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
  register()
