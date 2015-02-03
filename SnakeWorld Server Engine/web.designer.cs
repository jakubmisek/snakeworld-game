﻿#pragma warning disable 1591
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.3074
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace SnakeWorld_Server
{
	using System.Data.Linq;
	using System.Data.Linq.Mapping;
	using System.Data;
	using System.Collections.Generic;
	using System.Reflection;
	using System.Linq;
	using System.Linq.Expressions;
	using System.ComponentModel;
	using System;
	
	
	[System.Data.Linq.Mapping.DatabaseAttribute(Name="sacredware_web")]
	public partial class webDataContext : System.Data.Linq.DataContext
	{
		
		private static System.Data.Linq.Mapping.MappingSource mappingSource = new AttributeMappingSource();
		
    #region Extensibility Method Definitions
    partial void OnCreated();
    partial void InsertUserInfo(UserInfo instance);
    partial void UpdateUserInfo(UserInfo instance);
    partial void DeleteUserInfo(UserInfo instance);
    #endregion
		
		public webDataContext() : 
				base(global::SnakeWorld_Server.Properties.Settings.Default.sacredware_webConnectionString, mappingSource)
		{
			OnCreated();
		}
		
		public webDataContext(string connection) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public webDataContext(System.Data.IDbConnection connection) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public webDataContext(string connection, System.Data.Linq.Mapping.MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public webDataContext(System.Data.IDbConnection connection, System.Data.Linq.Mapping.MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public System.Data.Linq.Table<UserInfo> UserInfos
		{
			get
			{
				return this.GetTable<UserInfo>();
			}
		}
	}
	
	[Table(Name="dbo.UserInfo")]
	public partial class UserInfo : INotifyPropertyChanging, INotifyPropertyChanged
	{
		
		private static PropertyChangingEventArgs emptyChangingEventArgs = new PropertyChangingEventArgs(String.Empty);
		
		private int _userId;
		
		private string _email;
		
		private string _name;
		
		private bool _isAdmin;
		
		private string _passwordHash;
		
		private string _saltHash;
		
		private bool _isActive;
		
		private System.DateTime _dateRegistered;
		
    #region Extensibility Method Definitions
    partial void OnLoaded();
    partial void OnValidate(System.Data.Linq.ChangeAction action);
    partial void OnCreated();
    partial void OnuserIdChanging(int value);
    partial void OnuserIdChanged();
    partial void OnemailChanging(string value);
    partial void OnemailChanged();
    partial void OnnameChanging(string value);
    partial void OnnameChanged();
    partial void OnisAdminChanging(bool value);
    partial void OnisAdminChanged();
    partial void OnpasswordHashChanging(string value);
    partial void OnpasswordHashChanged();
    partial void OnsaltHashChanging(string value);
    partial void OnsaltHashChanged();
    partial void OnisActiveChanging(bool value);
    partial void OnisActiveChanged();
    partial void OndateRegisteredChanging(System.DateTime value);
    partial void OndateRegisteredChanged();
    #endregion
		
		public UserInfo()
		{
			OnCreated();
		}
		
		[Column(Storage="_userId", AutoSync=AutoSync.OnInsert, DbType="Int NOT NULL IDENTITY", IsPrimaryKey=true, IsDbGenerated=true)]
		public int userId
		{
			get
			{
				return this._userId;
			}
			set
			{
				if ((this._userId != value))
				{
					this.OnuserIdChanging(value);
					this.SendPropertyChanging();
					this._userId = value;
					this.SendPropertyChanged("userId");
					this.OnuserIdChanged();
				}
			}
		}
		
		[Column(Storage="_email", DbType="VarChar(255) NOT NULL", CanBeNull=false)]
		public string email
		{
			get
			{
				return this._email;
			}
			set
			{
				if ((this._email != value))
				{
					this.OnemailChanging(value);
					this.SendPropertyChanging();
					this._email = value;
					this.SendPropertyChanged("email");
					this.OnemailChanged();
				}
			}
		}
		
		[Column(Storage="_name", DbType="NVarChar(255) NOT NULL", CanBeNull=false)]
		public string name
		{
			get
			{
				return this._name;
			}
			set
			{
				if ((this._name != value))
				{
					this.OnnameChanging(value);
					this.SendPropertyChanging();
					this._name = value;
					this.SendPropertyChanged("name");
					this.OnnameChanged();
				}
			}
		}
		
		[Column(Storage="_isAdmin", DbType="Bit NOT NULL")]
		public bool isAdmin
		{
			get
			{
				return this._isAdmin;
			}
			set
			{
				if ((this._isAdmin != value))
				{
					this.OnisAdminChanging(value);
					this.SendPropertyChanging();
					this._isAdmin = value;
					this.SendPropertyChanged("isAdmin");
					this.OnisAdminChanged();
				}
			}
		}
		
		[Column(Storage="_passwordHash", DbType="VarChar(64) NOT NULL", CanBeNull=false)]
		public string passwordHash
		{
			get
			{
				return this._passwordHash;
			}
			set
			{
				if ((this._passwordHash != value))
				{
					this.OnpasswordHashChanging(value);
					this.SendPropertyChanging();
					this._passwordHash = value;
					this.SendPropertyChanged("passwordHash");
					this.OnpasswordHashChanged();
				}
			}
		}
		
		[Column(Storage="_saltHash", DbType="VarChar(50) NOT NULL", CanBeNull=false)]
		public string saltHash
		{
			get
			{
				return this._saltHash;
			}
			set
			{
				if ((this._saltHash != value))
				{
					this.OnsaltHashChanging(value);
					this.SendPropertyChanging();
					this._saltHash = value;
					this.SendPropertyChanged("saltHash");
					this.OnsaltHashChanged();
				}
			}
		}
		
		[Column(Storage="_isActive", DbType="Bit NOT NULL")]
		public bool isActive
		{
			get
			{
				return this._isActive;
			}
			set
			{
				if ((this._isActive != value))
				{
					this.OnisActiveChanging(value);
					this.SendPropertyChanging();
					this._isActive = value;
					this.SendPropertyChanged("isActive");
					this.OnisActiveChanged();
				}
			}
		}
		
		[Column(Storage="_dateRegistered", DbType="DateTime NOT NULL")]
		public System.DateTime dateRegistered
		{
			get
			{
				return this._dateRegistered;
			}
			set
			{
				if ((this._dateRegistered != value))
				{
					this.OndateRegisteredChanging(value);
					this.SendPropertyChanging();
					this._dateRegistered = value;
					this.SendPropertyChanged("dateRegistered");
					this.OndateRegisteredChanged();
				}
			}
		}
		
		public event PropertyChangingEventHandler PropertyChanging;
		
		public event PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			if ((this.PropertyChanging != null))
			{
				this.PropertyChanging(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(String propertyName)
		{
			if ((this.PropertyChanged != null))
			{
				this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
	}
}
#pragma warning restore 1591