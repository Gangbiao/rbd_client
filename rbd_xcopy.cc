#include <rados/librados.hpp>
#include <rbd/librbd.hpp>
#include <iostream>
#include <string>
#include <sstream>

/*
  1. creaet xcopy_pool 
  2. create librbd_test1, and write data
*/

void simple_xcopy_cb_pp(librbd::completion_t cb, void *arg)
{
  std::cout << "xcopy completion cb called!" << std::endl;
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

  {
    std::string name1 = "test1";
    std::string name2 = "test2";
    uint64_t size = 1 << 29;
    size *=10;
    int order = 0;
    librbd::RBD rbd;
    librbd::Image image1, image2; // to open librbd_test1 and librbd_test2

    //create an open "librbd_test2"
    /*ret = rbd.create(io_ctx, name2.c_str(), size, &order);
    if (ret < 0) {
      std::cerr << "couldn't create librbd_test2! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just created librbd_test2" << std::endl;
    }*/

    ret = rbd.open(io_ctx, image2, name2.c_str(), NULL);
    if (ret < 0) {
      std::cerr << "couldn't open the librbd_test2! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just opened librbd_test2" << std::endl;
    }

    ret = rbd.open(io_ctx, image1, name1.c_str(), NULL);
    if (ret < 0) {
      std::cerr << "couldn't open the librbd_test1! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just opened librbd_test1" << std::endl;
    }

    size_t offset=128;
    size_t len=256;
    char zero_data[512]={0};
    uint64_t mismatch_off = 1;
    int op_flags = 0;

    ceph::bufferlist zero_bl, bl;
    zero_bl.append(zero_data, 512);



    librbd::RBD::AioCompletion *comp = new librbd::RBD::AioCompletion(NULL, (librbd::callback_t) simple_xcopy_cb_pp);
    printf("created completion\n"); 
   
    //C_SaferCond cond;
    //AioCompletion *comp = AioCompletion::create(&cond);
 
    ret = image1.aio_xcopy(0, size, image1, size, comp, op_flags);
    if (ret < 0) {
      std::cerr << "couldn't call xcopy! error " << mismatch_off << std::endl;
      std::cerr << "image1.xcopy return ret =  " << ret << std::endl;
      //ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just call xcopy " << std::endl;
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

   /* image1.close();
    image2.close();

    ret = rbd.remove(io_ctx, name1.c_str());
    ret = rbd.remove(io_ctx, name2.c_str());*/
    if (ret < 0) {
      std::cerr << "failed to delete rbd image! error " << ret << std::endl;
      ret = EXIT_FAILURE;
      goto out;
    } else {
      std::cout << "we just deleted our rbd image " << std::endl;
    }
  }

  ret = EXIT_SUCCESS;
  out:
  /*int delete_ret = rados.pool_delete(pool_name);
  if (delete_ret < 0) {
    // be careful not to
    std::cerr << "We failed to delete our test pool!" << std::endl;
    ret = EXIT_FAILURE;
  }*/
  rados.shutdown();
  return ret;
}
