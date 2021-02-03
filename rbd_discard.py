import sys
sys.path.append('/var/lib/ceph/lib64/python2.7/site-packages/')
import rados
import rbd

if __name__=="__main__":

    # get the rbd image
    cluster = rados.Rados(conffile = '/var/lib/ceph/etc/ceph/ceph.conf')
    cluster.connect()
    ioctx = cluster.open_ioctx('rbd')
    rbd_inst = rbd.RBD()
    #print "==================rbd list================" 
    #print rbd_inst.list(ioctx);
    #print "==================rbd list_long================" 
    #print rbd_inst.list_long(ioctx);

    lun_name=sys.argv[1]
    rbd_image=rbd.Image(ioctx, lun_name)
    offset=0;
    while offset<rbd_image.size():
      rbd_image.discard(offset, 1073741824)  # 1GB
      offset+=1073741824
    #rbd_image.discard(10737418240, 10737418241)  # 100MB
    # source_rbd_image.create_snap('snap11')
    # source_rbd_image.set_snap('snap11')
    #interval_list, size = _get_incr_info(
    #        source_rbd_image, None, 0,
    #        source_rbd_image.size(), "backup_data_file")
    #print "interval_list"
    #print interval_list
    #print "size"
    #print size

