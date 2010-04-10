#ifndef CTKPLUGINSTORAGE_P_H
#define CTKPLUGINSTORAGE_P_H

#include "ctkPluginFrameworkContext.h"
#include "ctkPluginArchiveInterface_p.h"

namespace ctk {

  class PluginStorage {

  private:

    /**
     * Plugin id sorted list of all active plugin archives.
     */
    QList<PluginArchiveInterface*> archives;

     /**
      * Framework handle.
      */
     PluginFrameworkContext* framework;

  public:

     /**
      * Create a container for all plugin data in this framework.
      * Try to restore all saved plugin archive state.
      *
      */
     PluginStorage(PluginFrameworkContext* framework)
       : framework(framework)
     {
       // See if we have a storage database
       bundlesDir = Util.getFileStorage(framework, "bs");
       if (bundlesDir == null) {
         throw new RuntimeException("No bundle storage area available!");
       }
       // Restore all saved bundles
       String [] list = bundlesDir.list();
       for (int i = 0; list != null & i < list.length; i++) {
         long id;
         try {
           id = Long.parseLong(list[i]);
         } catch (NumberFormatException e) {
           continue;
         }
         if (id == 0) {
           System.err.println("Saved bundle with illegal id 0 is ignored.");
         }
         int pos = find(id);
         if (pos < archives.size() && ((BundleArchive)archives.get(pos)).getBundleId() == id) {
           System.err.println("There are two bundle directories with id: " + id);
           break;
         }
         FileTree dir = new FileTree(bundlesDir, list[i]);
         if (dir.isDirectory()) {
           try {
             boolean bUninstalled = BundleArchiveImpl.isUninstalled(dir);
             if(bUninstalled) {
               // silently remove any bundle marked as uninstalled
               dir.delete();
             } else {
               BundleArchive ba = new BundleArchiveImpl(this, dir, id);
               archives.add(pos, ba);
             }
             if (id >= nextFreeId) {
               nextFreeId = id + 1;
             }
           } catch (Exception e) {
             dir.delete();
             System.err.println("Removed corrupt bundle dir (" + e.getMessage() + "): " + dir);
           }
         }
       }
     }


     /**
      * Insert bundle into persistent storage
      *
      * @param location Location of bundle.
      * @param is Inputstrem with bundle content.
      * @return Bundle archive object.
      */
     public BundleArchive insertBundleJar(String location, InputStream is)
       throws Exception
     {
       long id = nextFreeId++;
       FileTree dir = new FileTree(bundlesDir, String.valueOf(id));
       if (dir.exists()) {
         // remove any old garbage
         dir.delete();
       }
       dir.mkdir();
       try {
         BundleArchive ba = new BundleArchiveImpl(this, dir, is, location, id);
         archives.add(ba);
         return ba;
       } catch (Exception e) {
         dir.delete();
         throw e;
       }
     }


     /**
      * Insert a new jar file into persistent storagedata as an update
      * to an existing bundle archive. To commit this data a call to
      * <code>replaceBundleArchive</code> is needed.
      *
      * @param old BundleArchive to be replaced.
      * @param is Inputstrem with bundle content.
      * @return Bundle archive object.
      */
     public BundleArchive updateBundleArchive(BundleArchive old, InputStream is)
       throws Exception
     {
       return new BundleArchiveImpl((BundleArchiveImpl)old, is);
     }


     /**
      * Replace old bundle archive with a new updated bundle archive, that
      * was created with updateBundleArchive.
      *
      * @param oldBA BundleArchive to be replaced.
      * @param newBA Inputstrem with bundle content.
      * @return New bundle archive object.
      */
     public void replaceBundleArchive(BundleArchive oldBA, BundleArchive newBA)
       throws Exception
     {
       int pos;
       long id = oldBA.getBundleId();
       synchronized (archives) {
         pos = find(id);
         if (pos >= archives.size() || archives.get(pos) != oldBA) {
           throw new Exception("replaceBundleJar: Old bundle archive not found, pos=" + pos);
         }
         archives.set(pos, newBA);
       }
     }


     /**
      * Get all bundle archive objects.
      *
      * @return Private array of all BundleArchives.
      */
     public BundleArchive [] getAllBundleArchives() {
       synchronized (archives) {
         return (BundleArchive [])archives.toArray(new BundleArchive[archives.size()]);
       }
     }


     /**
      * Get all bundles to start at next launch of framework.
      * This list is sorted in increasing bundle id order.
      *
      * @return Private copy of a List with bundle id's.
      */
     public List getStartOnLaunchBundles() {
       ArrayList res = new ArrayList();
       for (Iterator i = archives.iterator(); i.hasNext(); ) {
         BundleArchive ba = (BundleArchive)i.next();
         if (ba.getAutostartSetting()!=-1) {
           res.add(ba.getBundleLocation());
         }
       }
       return res;
     }


     /**
      * Close bundle storage.
      *
      */
     public void close()
     {
       for (Iterator i = archives.iterator(); i.hasNext(); ) {
         BundleArchive ba = (BundleArchive) i.next();
         ba.close();
         i.remove();
       }
       framework = null;
       bundlesDir = null;
     }


   private:

     /**
      * Remove bundle archive from archives list.
      *
      * @param id Bundle archive id to find.
      * @return true if element was removed.
      */
     bool removeArchive(PluginArchiveInterface* ba) {
       synchronized (archives) {
         int pos = find(ba.getBundleId());
         if (pos < archives.size() && archives.get(pos) == ba) {
           archives.remove(pos);
           return true;
         } else {
           return false;
         }
       }
     }

     /**
      * Find posisition for BundleArchive with specified id
      *
      * @param id Bundle archive id to find.
      * @return String to write
      */
     int find(long id) {
       int lb = 0;
       int ub = archives.size() - 1;
       int x = 0;
       while (lb < ub) {
         x = (lb + ub) / 2;
         long xid = ((BundleArchive)archives.get(x)).getBundleId();
         if (id == xid) {
           return x;
         } else if (id < xid) {
           ub = x;
         } else {
           lb = x+1;
         }
       }
       if (lb < archives.size() && ((BundleArchive)archives.get(lb)).getBundleId() < id) {
         return lb + 1;
       }
       return lb;
     }


  };

}

#endif // CTKPLUGINSTORAGE_P_H
