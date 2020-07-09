// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation. See file COPYING.
 */

// install the librados-dev and librbd package to get this
#include <rados/librados.hpp>
#include <rbd/librbd.hpp>
#include <iostream>
#include <string>
#include <sstream>

static int disk_usage_callback(uint64_t offset, size_t len, int exists,
                               void *arg) {
  uint64_t *used_size = reinterpret_cast<uint64_t *>(arg);
  if (exists) {
    (*used_size) += len;
  }
  return 0;
}

// diff_iterate_cb
//int diff_iterate_cb(uint64_t off, size_t len, int exists, void *arg)
//{
  //cout << "iterate_cb " << off << "~" << len << std::endl;
//  interval_set<uint64_t> *diff = static_cast<interval_set<uint64_t> *>(arg);
//  diff->insert(off, len);
//  return 0;
//}

//简单的回调函数，用于librbd::RBD::AioCompletion
void simple_write_cb(librbd::completion_t cb, void *arg) {
    std::cout << "write completion cb called!" << std::endl;
}

void simple_read_cb(librbd::completion_t cb, void *arg) {
    std::cout << "read completion cb called!" << std::endl;
}


int main(int argc, const char **argv)
{
  int ret = 0;

  // we will use all of these below
  const char *pool_name = "rbd";
  std::string hello("hello world!");
  std::string object_name("hello_object");
  librados::IoCtx io_ctx;

  // first, we create a Rados object and initialize it
  librados::Rados rados;
  {
    ret = rados.init("admin"); // just use the client.admin keyring
    if (ret < 0) { // let's handle any error that might have come back
      std::cerr << "couldn't initialize rados! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just set up a rados cluster object" << std::endl;
    }
  }

  /*
   * Now we need to get the rados object its config info. It can
   * parse argv for us to find the id, monitors, etc, so let's just
   * use that.
   */
  {
    ret = rados.conf_parse_argv(argc, argv);
    if (ret < 0) {
      // This really can't happen, but we need to check to be a good citizen.
      std::cerr << "failed to parse config options! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just parsed our config options" << std::endl;
      // We also want to apply the config file if the user specified
      // one, and conf_parse_argv won't do that for us.
      for (int i = 0; i < argc; ++i) {
	if ((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--conf") == 0)) {
	  ret = rados.conf_read_file(argv[i+1]);
	  if (ret < 0) {
	    // This could fail if the config file is malformed, but it'd be hard.
	    std::cerr << "failed to parse config file " << argv[i+1]
	              << "! error" << ret << std::endl;
	    ret = EXIT_FAILURE;
	    goto out;
	  }
	  break;
	}
      }
    }
  }

  /*
   * next, we actually connect to the cluster
   */
  {
    ret = rados.connect();
    if (ret < 0) {
      std::cerr << "couldn't connect to cluster! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just connected to the rados cluster" << std::endl;
    }
  }

  /*
   * let's create our own pool instead of scribbling over real data.
   * Note that this command creates pools with default PG counts specified
   * by the monitors, which may not be appropriate for real use -- it's fine
   * for testing, though.
   */
  /*{
    ret = rados.pool_create(pool_name);
    if (ret < 0) {
      std::cerr << "couldn't create pool! error " << ret << std::endl;
      return EXIT_FAILURE;
    } else {
      std::cout << "we just created a new pool named " << pool_name << std::endl;
    }
  }*/

  /*
   * create an "IoCtx" which is used to do IO to a pool
   */
  {
    ret = rados.ioctx_create(pool_name, io_ctx);
    if (ret < 0) {
      std::cerr << "couldn't set up ioctx! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just created an ioctx for our pool" << std::endl;
    }
  }

  /*
   * create an rbd image and write data to it
   */
  {
    std::string name = "test";
    uint64_t size = 1<<29;
    //size *=4;
    int order = 0;
    librbd::RBD rbd;
    librbd::Image image;

    /*ret = rbd.create(io_ctx, name.c_str(), size, &order);
    if (ret < 0) {
      std::cerr << "couldn't create an rbd image! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just created an rbd image" << std::endl;
    }*/

    ret = rbd.open(io_ctx, image, name.c_str(), NULL);
    if (ret < 0) {
      std::cerr << "couldn't open the rbd image! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just opened the rbd image" << std::endl;
    }
  
    size_t offset=128;
    size_t len=256;   
    char zero_data[256]={0};
    uint64_t mismatch_off = 1;
    int op_flags = 0;
    
    ceph::bufferlist zero_bl, bl;
    //zero_bl.append(zero_data, 1);
    //zero_bl.append_zero(size);
    librbd::RBD::AioCompletion *write_completion = new librbd::RBD::AioCompletion(
        NULL, (librbd::callback_t) simple_write_cb); //读写AioCompletion
    //ret = image.writesame(offset, len, cmp_bl, op_flags);
    //ret = image.aio_zero(0, size, write_completion, op_flags);
    //ret = image.aio_discard(0, size, write_completion);
    //ret = image.diff_iterate2(NULL, 0, size, true, this->whole_object,
    //                          iterate_cb, (void *)&diff);
    uint64_t *used_size=NULL;
    ret = image.diff_iterate2(NULL, 0, size, true, true,
                            &disk_usage_callback, used_size);

 
    if (ret < 0) {
      std::cerr << "couldn't compare and write to the rbd image! error " << mismatch_off << std::endl;
      //ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just write_zero to  our rbd image " << std::endl;
    }

    /*
     * let's read the image and compare it to the data we wrote
     */
    //ceph::bufferlist bl_r;
    //int read;
    //read = image.read(0, TEST_IO_SIZE, bl_r);
    //if (read < 0) {
    //  std::cerr << "we couldn't read data from the image! error" << std::endl;
    //  ret = EXIT_FAILURE;
    //  goto out;
    //}
  
    //std::string bl_res(bl_r.c_str(), read);
  
    //int res = memcmp(bl_res.c_str(), test_data, TEST_IO_SIZE);
    //if (res != 0) {
    //  std::cerr << "what we read didn't match expected! error" << std::endl;
    //} else {
    //  std::cout << "we read our data on the image successfully" << std::endl;
    //}

    image.close();

    /*
     *let's now delete the image
     */
    /*ret = rbd.remove(io_ctx, name.c_str());
    if (ret < 0) {
      std::cerr << "failed to delete rbd image! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just deleted our rbd image " << std::endl;
    }*/
  }

  ret = EXIT_SUCCESS;
  out:
  /*
   * And now we're done, so let's remove our pool and then
   * shut down the connection gracefully.
   */
  /*int delete_ret = rados.pool_delete(pool_name);
  if (delete_ret < 0) {
    // be careful not to
    std::cerr << "We failed to delete our test pool!" << std::endl;
    ret = EXIT_FAILURE;
  }*/

  rados.shutdown();

  return ret;
}
